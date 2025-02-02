#include <unistd.h>
#include <linux/uinput.h>
#include <string.h>
#include "include/input.h"
#include "include/log.h"

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
