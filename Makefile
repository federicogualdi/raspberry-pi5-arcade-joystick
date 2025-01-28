CFLAGS = -Wall
LDFLAGS = -llgpio

INCLUDE_DIR = include

SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

BUILD_DIR = build
TARGET = $(BUILD_DIR)/joystick

# Select compiler based on architecture
ifeq ($(UNAME_M), aarch64)
    CC = gcc  # Use the native GCC on aarch64
else
    CC = aarch64-linux-gnu-gcc  # Use the cross-compiler for aarch64
	# Use the libraries provided in this repo
	LIBRARY_PATH := $(shell pwd)/arcade-joystick-installer/usr/lib
	LDFLAGS = -L$(LIBRARY_PATH) -llgpio
endif

all: $(TARGET)
	@echo "Used compiler: $(CC)"

$(TARGET): $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean