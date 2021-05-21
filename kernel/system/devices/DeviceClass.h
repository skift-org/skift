#pragma once

#include <libsystem/Common.h>

#define DEVICE_CLASS_LIST(__ENTRY)    \
    __ENTRY(NONE, none)               \
    __ENTRY(FRAMEBUFFER, framebuffer) \
    __ENTRY(KEYBOARD, keyboard)       \
    __ENTRY(MOUSE, mouse)             \
    __ENTRY(SERIAL, serial)           \
    __ENTRY(DISK, disk)               \
    __ENTRY(PARTITION, part)          \
    __ENTRY(RANDOM, random)           \
    __ENTRY(NETWORK, network)         \
    __ENTRY(NULL_, null)              \
    __ENTRY(CONSOLE, console)         \
    __ENTRY(ZERO, zero)               \
    __ENTRY(SOUND, sound)             \
    __ENTRY(PCSPEAKER, speaker)

enum class DeviceClass : uint8_t
{
#define DEVICE_ENUM_ENTRY(__name, ...) __name,
    DEVICE_CLASS_LIST(DEVICE_ENUM_ENTRY)

        __COUNT,
};
