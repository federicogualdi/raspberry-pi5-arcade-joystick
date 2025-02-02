#ifndef JOYSTICK_H
#define JOYSTICK_H

int setup_uinput_device();
void cleanup_uinput_device(int fd);

#endif
