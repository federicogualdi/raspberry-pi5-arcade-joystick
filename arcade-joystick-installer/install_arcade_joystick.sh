#!/bin/bash

# Configurations
REPO_URL="https://github.com/federicogualdi/raspberry-pi5-arcade-joystick.git"
INSTALL_DIR="/recalbox/share/system/arcade_joystick"
EXECUTABLE_NAME="joystick"
LIB_NAME="liblgpio.so.1"
LIB_DEST="/usr/lib"

# Ensure we are running with root privileges
if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root (sudo ./install_arcade_joystick.sh)"
    exit 1
fi

echo "Installing Arcade Joystick for Raspberry Pi 5 on Recalbox..."

# Install git if not present (Recalbox has limited package support)
if ! command -v git &> /dev/null; then
    echo "Git not found, attempting to install..."
    mount -o remount,rw /
    mkdir -p /tmp/packages
    wget https://github.com/git/git/releases/download/v2.39.3/git-2.39.3.tar.gz -O /tmp/packages/git.tar.gz
    tar -xzf /tmp/packages/git.tar.gz -C /tmp/packages/
    mv /tmp/packages/git /usr/bin/git
    mount -o remount,ro /
    echo "Git installed successfully."
fi

# Remove old installation if exists
if [ -d "$INSTALL_DIR" ]; then
    echo "Removing old installation..."
    rm -rf "$INSTALL_DIR"
fi

# Clone the repository
echo "Cloning repository from $REPO_URL..."
git clone $REPO_URL $INSTALL_DIR

if [ $? -ne 0 ]; then
    echo "Failed to clone repository. Please check your internet connection or repository URL."
    exit 1
fi

echo "Repository cloned successfully."

# Copy the compiled joystick binary to the system folder
echo "Copying joystick binary to system folder..."
cp "$INSTALL_DIR/$EXECUTABLE_NAME" /recalbox/share/system/

# Ensure the binary is executable
chmod +x /recalbox/share/system/$EXECUTABLE_NAME

# Copy the lgpio library to the correct location
echo "Copying lgpio library to $LIB_DEST..."
cp "$INSTALL_DIR/$LIB_NAME" "$LIB_DEST"

# Make sure the library is discoverable
ldconfig

# Set up auto-start at boot
echo "Configuring auto-start for joystick..."
CUSTOM_SH="/recalbox/share/system/custom.sh"
if ! grep -q "$EXECUTABLE_NAME" "$CUSTOM_SH"; then
    echo "/recalbox/share/system/$EXECUTABLE_NAME &" >> "$CUSTOM_SH"
    echo "Auto-start configured successfully."
else
    echo "Auto-start already configured."
fi

# Finalizing
echo "Installation complete. Rebooting Recalbox to apply changes..."
reboot
