#pragma once

#include <libutils/Iter.h>

#include "ps2/LegacyAddress.h"

#define PS2_BUFFER 0x60
#define PS2_STATUS 0x64
#define PS2_ACK 0xFA
#define PS2_BUFFER_FULL 0x01
#define PS2_WHICH_BUFFER 0x20
#define PS2_MOUSE_BUFFER 0x20
#define PS2_KEYBOARD_BUFFER 0x00

Iter legacy_scan(IterFunc<LegacyAddress> callback);
