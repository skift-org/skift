#pragma once

#define DEVICE_PATH "/Devices"

#define KEYBOARD_DEVICE_PATH DEVICE_PATH "/keyboard"

#define KEYBOARD_EVENT_DEVICE_PATH DEVICE_PATH "/keyboard-event"

#define MOUSE_DEVICE_PATH DEVICE_PATH "/mouse"

#define TEXTMODE_DEVICE_PATH DEVICE_PATH "/textmode"

#define FRAMEBUFFER_DEVICE_PATH DEVICE_PATH "/framebuffer"

#define SERIAL_DEVICE_PATH DEVICE_PATH "/serial"

#define UNIX_DEVICE_PATH(__device) DEVICE_PATH "/" __device
