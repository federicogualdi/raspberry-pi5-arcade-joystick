#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <string.h>
#include "include/joystick.h"
#include "include/log.h"
#include "include/buttons.h"

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

// Cleanup: Destroy the virtual device
void cleanup_uinput_device(int fd) {
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
