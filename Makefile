CC = gcc
CFLAGS = -Wall
LDFLAGS = -llgpio
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TARGET = $(BUILD_DIR)/joystick

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)
