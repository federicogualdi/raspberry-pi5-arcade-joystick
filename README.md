# Raspberry Pi 5 Arcade Joystick

An easy-to-use program that allows you to turn Raspberry Pi 5 GPIO inputs into a virtual arcade joystick.
Specifically designed for **Recalbox**, but compatible with other retro gaming systems.

## 📋 Features

- 🕹️ Maps arcade-style joystick and buttons to Raspberry Pi 5 GPIO pins.
- 🔌 Emulates a USB joystick using the Linux **`uinput`** system.
- 🎮 Supports multiple button mappings and joystick directions.
- ⚡ Optimized for **Raspberry Pi 5** running **Recalbox 9.2.3**.
- 🛠️ Lightweight and efficient, with minimal CPU usage.

## Requirements

- Raspberry Pi 5
- Recalbox 9.2.3

## ⚙ Automatic Installation (Recalbox)

To install automatically on **Recalbox**, run the following command:

```bash
cd /tmp
curl -H 'Cache-Control: no-cache' -sSL https://raw.githubusercontent.com/federicogualdi/raspberry-pi5-arcade-joystick/main/arcade-joystick-installer/install_arcade_joystick.sh | bash
```


## 🏗 Manual Installation

### Requirements

- Raspberry Pi 5
- Raspberry Pi OS (for compilation)
- `lgpio` library (for GPIO interaction)

### 1. Install dependencies (on Raspberry Pi OS)

```bash
sudo apt update
sudo apt install liblgpio-dev
```

### 2. Compile the program

On **Raspberry Pi OS**, compile using:

```bash
make build
```

### 3. Transfer the executable and dependencies to Recalbox

```bash
scp joystick root@RECALBOX.local:/recalbox/share/system/arcade_joystick
scp /usr/lib/aarch64-linux-gnu/liblgpio.so root@RECALBOX.local:/usr/lib/
scp /usr/lib/aarch64-linux-gnu/liblgpio.so.1 root@RECALBOX.local:/usr/lib/
```

### 4. Running the program

SSH into Recalbox and start the joystick emulator manually:

```bash
ssh root@RECALBOX.local
/recalbox/share/system/arcade_joystick/joystick
```

Run the joystick emulator with DEBUG log:

```bash
/recalbox/share/system/arcade_joystick/joystick --log-level DEBUG
```

To run it at startup, add the following line to `custom.sh`:
> Until I find a better solution, this way works
```bash
cp /recalbox/share/system/arcade_joystick/joystick /tmp
chmod +x /tmp/joystick
/tmp/joystick &
```

## 🕹️ Wiring Guide

Connect your arcade joystick and buttons to the corresponding GPIO pins:

| Function | GPIO Pin | Button  |
|----------|---------|---------|
| UP       | 17      | Joystick UP  |
| DOWN     | 27      | Joystick DOWN |
| LEFT     | 22      | Joystick LEFT |
| RIGHT    | 23      | Joystick RIGHT |
| A        | 5       | Button A |
| B        | 6       | Button B |
| X        | 13      | Button X |
| Y        | 19      | Button Y |
| L1       | 26      | Button L1 |
| R1       | 21      | Button R1 |

Ensure you use **pull-down resistors** if needed to avoid floating inputs.

## 🛠 Troubleshooting

- **Joystick not detected on Recalbox?**
  - Ensure the `uinput` kernel module is loaded:

    ```bash
    modprobe uinput
    ```

  - Verify joystick detection using:

    ```bash
    ls /dev/input/
    ```

  - A `jsx` should be shown. 

- **Library not found error?**
  - Copy the required `liblgpio.so` and `liblgpio.so.1` to `/usr/lib` on Recalbox.