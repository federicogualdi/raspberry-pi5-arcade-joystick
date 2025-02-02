#include <lgpio.h>
#include <signal.h>
#include <unistd.h>
#include "include/gpio.h"
#include "include/log.h"
#include "include/input.h"
#include "include/buttons.h"
#include "include/config.h"

extern volatile sig_atomic_t running;

static void log_button_event(int gpio, int pressed) {
    log_message(LOG_LEVEL_DEBUG, "Button %s (GPIO %d)", 
                pressed ? "PRESSED" : "RELEASED", gpio);
}

// Callback function for GPIO state change
void gpio_callback(int num_alerts, lgGpioAlert_p alerts, void *userdata) {
    log_message(LOG_LEVEL_DEBUG, "GPIO callback triggered, num_alerts: %d", num_alerts);

    int fd = *(int *)userdata;

    for (int i = 0; i < num_alerts; i++) {
        for (int j = 0; j < NUM_BUTTONS; j++) {
            if (buttons[j].gpio_pin == alerts[i].report.gpio) {
                log_button_event(buttons[j].gpio_pin, alerts[i].report.level);
                send_key_event(fd, buttons[j].key_code, alerts[i].report.level == 0 ? 1 : 0);
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
