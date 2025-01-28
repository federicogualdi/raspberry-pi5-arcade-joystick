#!/bin/bash

# Configurations
REPO_NAME="raspberry-pi5-arcade-joystick"
RAW_REPO_URL="https://raw.githubusercontent.com/federicogualdi/$REPO_NAME/main/arcade-joystick-installer"

INSTALL_DIR="/recalbox/share/system/arcade_joystick"

EXECUTABLE_NAME="joystick"
LIB_PATH="usr/lib"
LIB_NAME="liblgpio.so"
LIB_NAME_1="liblgpio.so.1"
LIB_DEST="/usr/lib"

# Ensure we are running with root privileges
if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root (sudo ./install_arcade_joystick.sh)"
    exit 1
fi

echo "Installing Arcade Joystick for Raspberry Pi 5 on Recalbox..."

# Download the repository as files
TEMP_DIR="/tmp/$REPO_NAME"
mkdir -p "$TEMP_DIR"

echo "Downloading files from $RAW_REPO_URL..."

# File to be downloaded
FILES=(
    "$EXECUTABLE_NAME"
    "$LIB_PATH/$LIB_NAME"
    "$LIB_PATH/$LIB_NAME_1"
)

# Download each file
for FILE in "${FILES[@]}"; do
    # Create needed directories
    DIR=$(dirname "$FILE")
    mkdir -p "$TEMP_DIR/$DIR"

    # Download file
    wget -q --show-progress "$RAW_REPO_URL/$FILE" -O "$TEMP_DIR/$FILE"
done

# Mount filesystem as rw
mount -o remount,rw /

# Move the extracted folder to the install directory
mv "$TEMP_DIR" "$INSTALL_DIR"
echo "Downlaoded data moved to $INSTALL_DIR"

# Ensure the binary is executable
chmod +x $INSTALL_DIR/$EXECUTABLE_NAME

# Move the lgpio library to the correct location
echo "Moving lgpio library to $LIB_DEST..."
mv "$INSTALL_DIR/$LIB_PATH/$LIB_NAME" "$LIB_DEST"
mv "$INSTALL_DIR/$LIB_PATH/$LIB_NAME_1" "$LIB_DEST"

# Set up auto-start at boot
echo "Configuring auto-start for joystick..."
CUSTOM_SH="/recalbox/share/system/custom.sh"
if ! grep -q "$EXECUTABLE_NAME" "$CUSTOM_SH"; then
    echo "cp $INSTALL_DIR/$EXECUTABLE_NAME /tmp/" >> "$CUSTOM_SH"
    echo "chmod +x /tmp/joystick" >> "$CUSTOM_SH"
    echo "/tmp/joystick &" >> "$CUSTOM_SH"
    #echo "$INSTALL_DIR/$EXECUTABLE_NAME &"
    echo "Auto-start configured successfully."
else
    echo "Auto-start already configured."
fi

# Finalizing
echo "Installation complete. Rebooting Recalbox to apply changes..."
reboot
