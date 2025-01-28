#include <stdio.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <string.h>
#include <lgpio.h>
#include <signal.h>
#include "include/log.h"

// GPIO pin definitions for buttons
#define GPIO_UP 17
#define GPIO_DOWN 27
#define GPIO_LEFT 22
#define GPIO_RIGHT 23
#define GPIO_A 5
#define GPIO_B 6
#define GPIO_X 13
#define GPIO_Y 19
#define GPIO_L1 26
#define GPIO_R1 21

// Constants for axis limits and polling
#define AXIS_MIN 0
#define AXIS_MAX 255

// Customizable preferences for fine tuning
#define BTN_DEBOUNCE_USEC (30 * 1000)
#define INTERRUPT_WAIT_USEC (100 * 1000)

// Injected during the build process
#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP "unknown"
#endif

// Button mapping to Linux key codes
struct button_map {
    int gpio_pin;
    int key_code;
};

struct button_map buttons[] = {
    {GPIO_UP, KEY_UP},
    {GPIO_DOWN, KEY_DOWN},
    {GPIO_LEFT, KEY_LEFT},
    {GPIO_RIGHT, KEY_RIGHT},
    {GPIO_A, BTN_A},
    {GPIO_B, BTN_B},
    {GPIO_X, BTN_X},
    {GPIO_Y, BTN_Y},
    {GPIO_L1, BTN_TL},
    {GPIO_R1, BTN_TR}
};

#define NUM_BUTTONS (sizeof(buttons) / sizeof(buttons[0]))

volatile sig_atomic_t running = 1;

// Function to configure the virtual joystick
int setup_uinput_device() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        log_message(LOG_LEVEL_ERROR, "Error opening /dev/uinput");
        return -1;
    }

    // Enable key and axis events
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_ABS);

    // Enable button events
    for (int i = 0; i < NUM_BUTTONS; i++) {
        ioctl(fd, UI_SET_KEYBIT, buttons[i].key_code);
    }

    // Configure joystick axes
    struct uinput_abs_setup abs_x = { ABS_X, {AXIS_MIN, AXIS_MAX, 0, 0} };
    struct uinput_abs_setup abs_y = { ABS_Y, {AXIS_MIN, AXIS_MAX, 0, 0} };
    ioctl(fd, UI_ABS_SETUP, &abs_x);
    ioctl(fd, UI_ABS_SETUP, &abs_y);

    // Configure the virtual device
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "GPIO Arcade Joystick");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    log_message(LOG_LEVEL_INFO, "GPIO Arcade Joystick successfully created!");
    return fd;
}

// Function to send key press and release events
void send_key_event(int fd, int key_code, int pressed) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = key_code;
    ev.value = pressed;
    write(fd, &ev, sizeof(ev));

    // Synchronization event
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

// Callback function for GPIO state change
void gpio_callback(int num_alerts, lgGpioAlert_p alerts, void *userdata) {
    log_message(LOG_LEVEL_DEBUG, "GPIO callback triggered, num_alerts: %d", num_alerts);

    int fd = *(int *)userdata;

    for (int i = 0; i < num_alerts; i++) {
        for (int j = 0; j < NUM_BUTTONS; j++) {
            if (buttons[j].gpio_pin == alerts[i].report.gpio) {
                if (alerts[i].report.level == 0) {  // Button pressed
                    log_message(LOG_LEVEL_DEBUG, "Button %d pressed", alerts[i].report.gpio);
                    send_key_event(fd, buttons[j].key_code, 1);
                } else {  // Button released
                    log_message(LOG_LEVEL_DEBUG, "Button %d released", alerts[i].report.gpio);
                    send_key_event(fd, buttons[j].key_code, 0);
                }
                break;
            }
        }
    }
}

// Function to monitor GPIOs using interrupts
void monitor_gpio(int fd) {
    log_message(LOG_LEVEL_INFO, "Starting GPIO monitoring...");
    int chip = lgGpiochipOpen(0);
    if (chip < 0) {
        log_message(LOG_LEVEL_ERROR, "Error opening GPIO chip");
        return;
    }    

    // Configure GPIO pins as input with pull-up and attach callback
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (
            lgGpioClaimAlert(chip, LG_SET_PULL_UP, LG_BOTH_EDGES, buttons[i].gpio_pin, -1) < 0 ||
            lgGpioSetAlertsFunc(chip, buttons[i].gpio_pin, gpio_callback, &fd) < 0 ||
            lgGpioSetDebounce(chip, buttons[i].gpio_pin, BTN_DEBOUNCE_USEC) < 0
        ) {
            log_message(LOG_LEVEL_ERROR, "Error while configuring GPIO pin");
            lgGpiochipClose(chip);
            return;
        }
    }
    log_message(LOG_LEVEL_INFO, "GPIO monitoring successfully set-up!");

    // Wait indefinitely until interrupted
    while (running) {
        usleep(INTERRUPT_WAIT_USEC);  // Sleep to avoid busy-waiting
    }

    lgGpiochipClose(chip);
}

// Function to handle SIGINT (Ctrl+C) signal
void handle_sigint(int sig) {
    running = 0;
    log_message(LOG_LEVEL_INFO, "Shutting down due to SIGINT...");
}

int main(int argc, char *argv[]) {
    set_log_level_from_args(argc, argv);
    signal(SIGINT, handle_sigint);

    log_message(LOG_LEVEL_INFO, "GPIO Arcade Joystick - Version: %s", BUILD_TIMESTAMP);
    
    int fd = setup_uinput_device();
    if (fd < 0) {
        return -1;
    }

    monitor_gpio(fd);

    // Cleanup: Destroy the virtual device before exiting
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    return 0;
}