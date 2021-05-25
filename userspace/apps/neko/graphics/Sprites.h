#pragma once

#include <libutils/Prelude.h>

namespace neko
{

enum struct Sprite : uint8_t
{
    AWAKE = 0x00,

    JARE = 0x30,

    KAKI1 = 0x31,
    KAKI2 = 0x32,

    MATI1 = 0x40,
    MATI2 = 0x41,

    SLEEP1 = 0x24,
    SLEEP2 = 0x34,

    UP1 = 0x50,
    UP2 = 0x44,

    DOWN1 = 0x10,
    DOWN2 = 0x01,

    LEFT1 = 0x03,
    LEFT2 = 0x13,

    RIGHT1 = 0x42,
    RIGHT2 = 0x43,

    UPLEFT1 = 0x51,
    UPLEFT2 = 0x52,

    UPRIGHT1 = 0x53,
    UPRIGHT2 = 0x54,

    DWLEFT1 = 0x21,
    DWLEFT2 = 0x02,

    DWRIGHT1 = 0x12,
    DWRIGHT2 = 0x22,

    UTOGI1 = 0x05,
    UTOGI2 = 0x15,

    DTOGI1 = 0x20,
    DTOGI2 = 0x11,

    LTOGI1 = 0x23,
    LTOGI2 = 0x33,

    RTOGI1 = 0x04,
    RTOGI2 = 0x14,
};

} // namespace neko
