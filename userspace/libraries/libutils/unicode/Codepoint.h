#pragma once

#include <libsystem/Common.h>

typedef uint32_t Codepoint;

inline bool codepoint_is_digit(Codepoint codepoint)
{
    return (codepoint >= U'0' && codepoint <= U'9');
}

inline bool codepoint_is_alpha(Codepoint codepoint)
{
    return (codepoint >= U'a' && codepoint <= U'z') ||
           (codepoint >= U'A' && codepoint <= U'Z');
}

inline int codepoint_numeric_value(Codepoint codepoint)
{
    if (codepoint_is_digit(codepoint))
    {
        return codepoint - U'0';
    }
    else
    {
        return 0;
    }
}

inline int utf8_to_codepoint(const uint8_t *buffer, Codepoint *codepoint)
{
    if ((buffer[0] & 0xf8) == 0xf0)
    {
        *codepoint = ((0x07 & buffer[0]) << 18) |
                     ((0x3f & buffer[1]) << 12) |
                     ((0x3f & buffer[2]) << 6) |
                     ((0x3f & buffer[3]));

        return 4;
    }
    else if ((buffer[0] & 0xf0) == 0xe0)
    {
        *codepoint = ((0x0f & buffer[0]) << 12) |
                     ((0x3f & buffer[1]) << 6) |
                     ((0x3f & buffer[2]));

        return 3;
    }
    else if ((buffer[0] & 0xe0) == 0xc0)
    {
        *codepoint = ((0x1f & buffer[0]) << 6) |
                     ((0x3f & buffer[1]));

        return 2;
    }
    else
    {
        *codepoint = buffer[0];

        return 1;
    }

    return 0;
}

inline int codepoint_to_utf8(Codepoint codepoint, uint8_t *buffer)
{
    if (codepoint <= 0x7F)
    {
        buffer[0] = (uint8_t)codepoint;
        buffer[1] = 0;

        return 1;
    }
    else if (codepoint <= 0x07FF)
    {
        buffer[0] = (uint8_t)(((codepoint >> 6) & 0x1F) | 0xC0);
        buffer[1] = (uint8_t)(((codepoint >> 0) & 0x3F) | 0x80);
        buffer[2] = 0;

        return 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        buffer[0] = (uint8_t)(((codepoint >> 12) & 0x0F) | 0xE0);
        buffer[1] = (uint8_t)(((codepoint >> 6) & 0x3F) | 0x80);
        buffer[2] = (uint8_t)(((codepoint >> 0) & 0x3F) | 0x80);
        buffer[3] = 0;

        return 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        buffer[0] = (uint8_t)(((codepoint >> 18) & 0x07) | 0xF0);
        buffer[1] = (uint8_t)(((codepoint >> 12) & 0x3F) | 0x80);
        buffer[2] = (uint8_t)(((codepoint >> 6) & 0x3F) | 0x80);
        buffer[3] = (uint8_t)(((codepoint >> 0) & 0x3F) | 0x80);
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
inline void codepoint_foreach(const uint8_t *buffer, T callback)
{
    Codepoint codepoint = 0;

    size_t size = utf8_to_codepoint(buffer, &codepoint);
    buffer += size;

    while (size && codepoint != 0)
    {
        callback(codepoint);

        size = utf8_to_codepoint(buffer, &codepoint);
        buffer += size;
    }
}
