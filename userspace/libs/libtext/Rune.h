#pragma once

#include <libutils/Prelude.h>

namespace Text
{

typedef uint32_t Rune;

inline bool rune_is_digit(Rune rune)
{
    return (rune >= U'0' && rune <= U'9');
}

inline bool rune_is_alpha(Rune rune)
{
    return (rune >= U'a' && rune <= U'z') ||
           (rune >= U'A' && rune <= U'Z');
}

inline int rune_numeric_value(Rune rune)
{
    if (rune_is_digit(rune))
    {
        return rune - U'0';
    }
    else
    {
        return 0;
    }
}

inline int utf8_to_rune(const uint8_t *buffer, Rune *rune)
{
    if ((buffer[0] & 0xf8) == 0xf0)
    {
        *rune = ((0x07 & buffer[0]) << 18) |
                ((0x3f & buffer[1]) << 12) |
                ((0x3f & buffer[2]) << 6) |
                ((0x3f & buffer[3]));

        return 4;
    }
    else if ((buffer[0] & 0xf0) == 0xe0)
    {
        *rune = ((0x0f & buffer[0]) << 12) |
                ((0x3f & buffer[1]) << 6) |
                ((0x3f & buffer[2]));

        return 3;
    }
    else if ((buffer[0] & 0xe0) == 0xc0)
    {
        *rune = ((0x1f & buffer[0]) << 6) |
                ((0x3f & buffer[1]));

        return 2;
    }
    else
    {
        *rune = buffer[0];

        return 1;
    }

    return 0;
}

inline int rune_to_utf8(Rune rune, uint8_t *buffer)
{
    if (rune <= 0x7F)
    {
        buffer[0] = (uint8_t)rune;
        buffer[1] = 0;

        return 1;
    }
    else if (rune <= 0x07FF)
    {
        buffer[0] = (uint8_t)(((rune >> 6) & 0x1F) | 0xC0);
        buffer[1] = (uint8_t)(((rune >> 0) & 0x3F) | 0x80);
        buffer[2] = 0;

        return 2;
    }
    else if (rune <= 0xFFFF)
    {
        buffer[0] = (uint8_t)(((rune >> 12) & 0x0F) | 0xE0);
        buffer[1] = (uint8_t)(((rune >> 6) & 0x3F) | 0x80);
        buffer[2] = (uint8_t)(((rune >> 0) & 0x3F) | 0x80);
        buffer[3] = 0;

        return 3;
    }
    else if (rune <= 0x10FFFF)
    {
        buffer[0] = (uint8_t)(((rune >> 18) & 0x07) | 0xF0);
        buffer[1] = (uint8_t)(((rune >> 12) & 0x3F) | 0x80);
        buffer[2] = (uint8_t)(((rune >> 6) & 0x3F) | 0x80);
        buffer[3] = (uint8_t)(((rune >> 0) & 0x3F) | 0x80);
        buffer[4] = 0;

        return 4;
    }
    else
    {
        buffer[0] = (uint8_t)0xEF;
        buffer[1] = (uint8_t)0xBF;
        buffer[2] = (uint8_t)0xBD;
        buffer[3] = 0;

        return 0;
    }
}

template <typename T>
inline void rune_foreach(const uint8_t *buffer, T callback)
{
    Rune rune = 0;

    size_t size = utf8_to_rune(buffer, &rune);
    buffer += size;

    while (size && rune != 0)
    {
        callback(rune);

        size = utf8_to_rune(buffer, &rune);
        buffer += size;
    }
}

} // namespace Text
