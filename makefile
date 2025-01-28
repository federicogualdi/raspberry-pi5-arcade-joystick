CC = gcc
CFLAGS = -Wall
LIB = -llgpio

build:
	$(CC) $(CFLAGS) -o joystick joystick.c $(LIB)
