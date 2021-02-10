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

inline char codepoint_to_cp437(Codepoint codepoint)
{
    if (codepoint >= U'\x20' && codepoint < U'\x7f')
    {
        return (char)codepoint;
    }

    switch (codepoint)
    {
    case U'☺':
        return 0x1;
    case U'☻':
        return 0x2;
    case U'♥':
        return 0x3;
    case U'♦':
        return 0x4;
    case U'♣':
        return 0x5;
    case U'♠':
        return 0x6;
    case U'•':
        return 0x7;
    case U'◘':
        return 0x8;
    case U'○':
        return 0x9;
    case U'◙':
        return 0xa;
    case U'♂':
        return 0xb;
    case U'♀':
        return 0xc;
    case U'♪':
        return 0xd;
    case U'♫':
        return 0xe;
    case U'☼':
        return 0xf;
    case U'►':
        return 0x10;
    case U'◄':
        return 0x11;
    case U'↕':
        return 0x12;
    case U'‼':
        return 0x13;
    case U'¶':
        return 0x14;
    case U'§':
        return 0x15;
    case U'▬':
        return 0x16;
    case U'↨':
        return 0x17;
    case U'↑':
        return 0x18;
    case U'↓':
        return 0x19;
    case U'→':
        return 0x1a;
    case U'←':
        return 0x1b;
    case U'∟':
        return 0x1c;
    case U'↔':
        return 0x1d;
    case U'▲':
        return 0x1e;
    case U'▼':
        return 0x1f;
    case U'⌂':
        return 0x7f;
    case U'Ç':
        return 0x80;
    case U'ü':
        return 0x81;
    case U'é':
        return 0x82;
    case U'â':
        return 0x83;
    case U'ä':
        return 0x84;
    case U'à':
        return 0x85;
    case U'å':
        return 0x86;
    case U'ç':
        return 0x87;
    case U'ê':
        return 0x88;
    case U'ë':
        return 0x89;
    case U'è':
        return 0x8a;
    case U'ï':
        return 0x8b;
    case U'î':
        return 0x8c;
    case U'ì':
        return 0x8d;
    case U'Ä':
        return 0x8e;
    case U'Å':
        return 0x8f;
    case U'É':
        return 0x90;
    case U'æ':
        return 0x91;
    case U'Æ':
        return 0x92;
    case U'ô':
        return 0x93;
    case U'ö':
        return 0x94;
    case U'ò':
        return 0x95;
    case U'û':
        return 0x96;
    case U'ù':
        return 0x97;
    case U'ÿ':
        return 0x98;
    case U'Ö':
        return 0x99;
    case U'Ü':
        return 0x9a;
    case U'¢':
        return 0x9b;
    case U'£':
        return 0x9c;
    case U'¥':
        return 0x9d;
    case U'₧':
        return 0x9e;
    case U'ƒ':
        return 0x9f;
    case U'á':
        return 0xa0;
    case U'í':
        return 0xa1;
    case U'ó':
        return 0xa2;
    case U'ú':
        return 0xa3;
    case U'ñ':
        return 0xa4;
    case U'Ñ':
        return 0xa5;
    case U'ª':
        return 0xa6;
    case U'º':
        return 0xa7;
    case U'¿':
        return 0xa8;
    case U'⌐':
        return 0xa9;
    case U'¬':
        return 0xaa;
    case U'½':
        return 0xab;
    case U'¼':
        return 0xac;
    case U'¡':
        return 0xad;
    case U'«':
        return 0xae;
    case U'»':
        return 0xaf;
    case U'░':
        return 0xb0;
    case U'▒':
        return 0xb1;
    case U'▓':
        return 0xb2;
    case U'│':
        return 0xb3;
    case U'┤':
        return 0xb4;
    case U'╡':
        return 0xb5;
    case U'╢':
        return 0xb6;
    case U'╖':
        return 0xb7;
    case U'╕':
        return 0xb8;
    case U'╣':
        return 0xb9;
    case U'║':
        return 0xba;
    case U'╗':
        return 0xbb;
    case U'╝':
        return 0xbc;
    case U'╜':
        return 0xbd;
    case U'╛':
        return 0xbe;
    case U'┐':
        return 0xbf;
    case U'└':
        return 0xc0;
    case U'┴':
        return 0xc1;
    case U'┬':
        return 0xc2;
    case U'├':
        return 0xc3;
    case U'─':
        return 0xc4;
    case U'┼':
        return 0xc5;
    case U'╞':
        return 0xc6;
    case U'╟':
        return 0xc7;
    case U'╚':
        return 0xc8;
    case U'╔':
        return 0xc9;
    case U'╩':
        return 0xca;
    case U'╦':
        return 0xcb;
    case U'╠':
        return 0xcc;
    case U'═':
        return 0xcd;
    case U'╬':
        return 0xce;
    case U'╧':
        return 0xcf;
    case U'╨':
        return 0xd0;
    case U'╤':
        return 0xd1;
    case U'╥':
        return 0xd2;
    case U'╙':
        return 0xd3;
    case U'╘':
        return 0xd4;
    case U'╒':
        return 0xd5;
    case U'╓':
        return 0xd6;
    case U'╫':
        return 0xd7;
    case U'╪':
        return 0xd8;
    case U'┘':
        return 0xd9;
    case U'┌':
        return 0xda;
    case U'█':
        return 0xdb;
    case U'▄':
        return 0xdc;
    case U'▌':
        return 0xdd;
    case U'▐':
        return 0xde;
    case U'▀':
        return 0xdf;
    case U'α':
        return 0xe0;
    case U'ß':
        return 0xe1;
    case U'Γ':
        return 0xe2;
    case U'π':
        return 0xe3;
    case U'Σ':
        return 0xe4;
    case U'σ':
        return 0xe5;
    case U'µ':
        return 0xe6;
    case U'τ':
        return 0xe7;
    case U'Φ':
        return 0xe8;
    case U'Θ':
        return 0xe9;
    case U'Ω':
        return 0xea;
    case U'δ':
        return 0xeb;
    case U'∞':
        return 0xec;
    case U'φ':
        return 0xed;
    case U'ε':
        return 0xee;
    case U'∩':
        return 0xef;
    case U'≡':
        return 0xf0;
    case U'±':
        return 0xf1;
    case U'≥':
        return 0xf2;
    case U'≤':
        return 0xf3;
    case U'⌠':
        return 0xf4;
    case U'⌡':
        return 0xf5;
    case U'÷':
        return 0xf6;
    case U'≈':
        return 0xf7;
    case U'°':
        return 0xf8;
    case U'∙':
        return 0xf9;
    case U'·':
        return 0xfa;
    case U'√':
        return 0xfb;
    case U'ⁿ':
        return 0xfc;
    case U'²':
        return 0xfd;
    case U'■':
        return 0xfe;

    default:
        return '?';
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
