#include <stdio.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <string.h>
#include <lgpio.h>
#include <signal.h>
#include "include/log.h"
#include "include/config.h"
#include "include/gpio.h"
#include "include/joystick.h"

volatile sig_atomic_t running = 1;

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

    cleanup_uinput_device(fd);
    return 0;
}