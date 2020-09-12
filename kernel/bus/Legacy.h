#pragma once

#include <libutils/Iteration.h>

#include "kernel/bus/LegacyAddress.h"

#define PS2_BUFFER 0x60
#define PS2_STATUS 0x64
#define PS2_ACK 0xFA
#define PS2_BUFFER_FULL 0x01
#define PS2_WHICH_BUFFER 0x20
#define PS2_MOUSE_BUFFER 0x20
#define PS2_KEYBOARD_BUFFER 0x00

Iteration legacy_scan(IterationCallback<LegacyAddress> callback);
