CFLAGS = -Wall
LDFLAGS = -llgpio

INCLUDE_DIR = include

SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

BUILD_DIR = build
TARGET = $(BUILD_DIR)/joystick

# Get the current timestamp in UTC
BUILD_TIMESTAMP := $(shell date -u +"%Y-%m-%d_%H:%M:%S")
# Add the UTC timestamp as a macro for the compiler
CFLAGS += -DBUILD_TIMESTAMP=\"$(BUILD_TIMESTAMP)\"

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

install-hooks:
	cp hooks/pre-commit .git/hooks/
	chmod +x .git/hooks/pre-commit

.PHONY: all clean install-hooks