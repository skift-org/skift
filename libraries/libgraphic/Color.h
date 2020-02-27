#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef union {
    struct
    {
        ubyte R;
        ubyte G;
        ubyte B;
        ubyte A;
    };

    uint packed;
} Color;

Color RGB(float R, float G, float B);

Color RGBA(float R, float G, float B, float A);

Color HSV(float H, float S, float V);

Color HSVA(float H, float S, float V, float A);

Color ALPHA(Color color, float alpha);

Color color_from_name(const char *name);

void color_get_name(Color color, char *name, int size);

Color color_lerp(Color a, Color b, float transition);

Color color_blerp(Color c00, Color c10, Color c01, Color c11, float transitionx, float transitiony);

Color color_blend(Color fg, Color bg);

#define COLOR(__value) ((Color){{(uchar)((__value) >> 16), (uchar)((__value) >> 8), (uchar)((__value)), 255}})

#define COLOR_ENUM(__ENTRY)                                               \
    __ENTRY(ALICEBLUE, AliceBlue, 0xF0, 0xF8, 0xFF)                       \
    __ENTRY(ANTIQUEWHITE, AntiqueWhite, 0xFA, 0xEB, 0xD7)                 \
    __ENTRY(AQUA, Aqua, 0x00, 0xFF, 0xFF)                                 \
    __ENTRY(AQUAMARINE, Aquamarine, 0x7F, 0xFF, 0xD4)                     \
    __ENTRY(AZURE, Azure, 0xF0, 0xFF, 0xFF)                               \
    __ENTRY(BEIGE, Beige, 0xF5, 0xF5, 0xDC)                               \
    __ENTRY(BISQUE, Bisque, 0xFF, 0xE4, 0xC4)                             \
    __ENTRY(BLACK, Black, 0x00, 0x00, 0x00)                               \
    __ENTRY(BLANCHEDALMOND, BlanchedAlmond, 0xFF, 0xEB, 0xCD)             \
    __ENTRY(BLUE, Blue, 0x00, 0x00, 0xFF)                                 \
    __ENTRY(BLUEVIOLET, BlueViolet, 0x8A, 0x2B, 0xE2)                     \
    __ENTRY(BROWN, Brown, 0xA5, 0x2A, 0x2A)                               \
    __ENTRY(BURLYWOOD, BurlyWood, 0xDE, 0xB8, 0x87)                       \
    __ENTRY(CADETBLUE, CadetBlue, 0x5F, 0x9E, 0xA0)                       \
    __ENTRY(CHARTREUSE, Chartreuse, 0x7F, 0xFF, 0x00)                     \
    __ENTRY(CHOCOLATE, Chocolate, 0xD2, 0x69, 0x1E)                       \
    __ENTRY(CORAL, Coral, 0xFF, 0x7F, 0x50)                               \
    __ENTRY(CORNFLOWERBLUE, CornflowerBlue, 0x64, 0x95, 0xED)             \
    __ENTRY(CORNSILK, Cornsilk, 0xFF, 0xF8, 0xDC)                         \
    __ENTRY(CRIMSON, Crimson, 0xDC, 0x14, 0x3C)                           \
    __ENTRY(CYAN, Cyan, 0x00, 0xFF, 0xFF)                                 \
    __ENTRY(DARKBLUE, DarkBlue, 0x00, 0x00, 0x8B)                         \
    __ENTRY(DARKCYAN, DarkCyan, 0x00, 0x8B, 0x8B)                         \
    __ENTRY(DARKGOLDENROD, DarkGoldenRod, 0xB8, 0x86, 0x0B)               \
    __ENTRY(DARKGRAY, DarkGray, 0xA9, 0xA9, 0xA9)                         \
    __ENTRY(DARKGREY, DarkGrey, 0xA9, 0xA9, 0xA9)                         \
    __ENTRY(DARKGREEN, DarkGreen, 0x00, 0x64, 0x00)                       \
    __ENTRY(DARKKHAKI, DarkKhaki, 0xBD, 0xB7, 0x6B)                       \
    __ENTRY(DARKMAGENTA, DarkMagenta, 0x8B, 0x00, 0x8B)                   \
    __ENTRY(DARKOLIVEGREEN, DarkOliveGreen, 0x55, 0x6B, 0x2F)             \
    __ENTRY(DARKORANGE, DarkOrange, 0xFF, 0x8C, 0x00)                     \
    __ENTRY(DARKORCHID, DarkOrchid, 0x99, 0x32, 0xCC)                     \
    __ENTRY(DARKRED, DarkRed, 0x8B, 0x00, 0x00)                           \
    __ENTRY(DARKSALMON, DarkSalmon, 0xE9, 0x96, 0x7A)                     \
    __ENTRY(DARKSEAGREEN, DarkSeaGreen, 0x8F, 0xBC, 0x8F)                 \
    __ENTRY(DARKSLATEBLUE, DarkSlateBlue, 0x48, 0x3D, 0x8B)               \
    __ENTRY(DARKSLATEGRAY, DarkSlateGray, 0x2F, 0x4F, 0x4F)               \
    __ENTRY(DARKSLATEGREY, DarkSlateGrey, 0x2F, 0x4F, 0x4F)               \
    __ENTRY(DARKTURQUOISE, DarkTurquoise, 0x00, 0xCE, 0xD1)               \
    __ENTRY(DARKVIOLET, DarkViolet, 0x94, 0x00, 0xD3)                     \
    __ENTRY(DEEPPINK, DeepPink, 0xFF, 0x14, 0x93)                         \
    __ENTRY(DEEPSKYBLUE, DeepSkyBlue, 0x00, 0xBF, 0xFF)                   \
    __ENTRY(DIMGRAY, DimGray, 0x69, 0x69, 0x69)                           \
    __ENTRY(DIMGREY, DimGrey, 0x69, 0x69, 0x69)                           \
    __ENTRY(DODGERBLUE, DodgerBlue, 0x1E, 0x90, 0xFF)                     \
    __ENTRY(FIREBRICK, FireBrick, 0xB2, 0x22, 0x22)                       \
    __ENTRY(FLORALWHITE, FloralWhite, 0xFF, 0xFA, 0xF0)                   \
    __ENTRY(FORESTGREEN, ForestGreen, 0x22, 0x8B, 0x22)                   \
    __ENTRY(FUCHSIA, Fuchsia, 0xFF, 0x00, 0xFF)                           \
    __ENTRY(GAINSBORO, Gainsboro, 0xDC, 0xDC, 0xDC)                       \
    __ENTRY(GHOSTWHITE, GhostWhite, 0xF8, 0xF8, 0xFF)                     \
    __ENTRY(GOLD, Gold, 0xFF, 0xD7, 0x00)                                 \
    __ENTRY(GOLDENROD, GoldenRod, 0xDA, 0xA5, 0x20)                       \
    __ENTRY(GRAY, Gray, 0x80, 0x80, 0x80)                                 \
    __ENTRY(GREY, Grey, 0x80, 0x80, 0x80)                                 \
    __ENTRY(GREEN, Green, 0x00, 0x80, 0x00)                               \
    __ENTRY(GREENYELLOW, GreenYellow, 0xAD, 0xFF, 0x2F)                   \
    __ENTRY(HONEYDEW, HoneyDew, 0xF0, 0xFF, 0xF0)                         \
    __ENTRY(HOTPINK, HotPink, 0xFF, 0x69, 0xB4)                           \
    __ENTRY(INDIANRED, IndianRed, 0xCD, 0x5C, 0x5C)                       \
    __ENTRY(INDIGO, Indigo, 0x4B, 0x00, 0x82)                             \
    __ENTRY(IVORY, Ivory, 0xFF, 0xFF, 0xF0)                               \
    __ENTRY(KHAKI, Khaki, 0xF0, 0xE6, 0x8C)                               \
    __ENTRY(LAVENDER, Lavender, 0xE6, 0xE6, 0xFA)                         \
    __ENTRY(LAVENDERBLUSH, LavenderBlush, 0xFF, 0xF0, 0xF5)               \
    __ENTRY(LAWNGREEN, LawnGreen, 0x7C, 0xFC, 0x00)                       \
    __ENTRY(LEMONCHIFFON, LemonChiffon, 0xFF, 0xFA, 0xCD)                 \
    __ENTRY(LIGHTBLUE, LightBlue, 0xAD, 0xD8, 0xE6)                       \
    __ENTRY(LIGHTCORAL, LightCoral, 0xF0, 0x80, 0x80)                     \
    __ENTRY(LIGHTCYAN, LightCyan, 0xE0, 0xFF, 0xFF)                       \
    __ENTRY(LIGHTGOLDENRODYELLOW, LightGoldenRodYellow, 0xFA, 0xFA, 0xD2) \
    __ENTRY(LIGHTGRAY, LightGray, 0xD3, 0xD3, 0xD3)                       \
    __ENTRY(LIGHTGREY, LightGrey, 0xD3, 0xD3, 0xD3)                       \
    __ENTRY(LIGHTGREEN, LightGreen, 0x90, 0xEE, 0x90)                     \
    __ENTRY(LIGHTPINK, LightPink, 0xFF, 0xB6, 0xC1)                       \
    __ENTRY(LIGHTSALMON, LightSalmon, 0xFF, 0xA0, 0x7A)                   \
    __ENTRY(LIGHTSEAGREEN, LightSeaGreen, 0x20, 0xB2, 0xAA)               \
    __ENTRY(LIGHTSKYBLUE, LightSkyBlue, 0x87, 0xCE, 0xFA)                 \
    __ENTRY(LIGHTSLATEGRAY, LightSlateGray, 0x77, 0x88, 0x99)             \
    __ENTRY(LIGHTSLATEGREY, LightSlateGrey, 0x77, 0x88, 0x99)             \
    __ENTRY(LIGHTSTEELBLUE, LightSteelBlue, 0xB0, 0xC4, 0xDE)             \
    __ENTRY(LIGHTYELLOW, LightYellow, 0xFF, 0xFF, 0xE0)                   \
    __ENTRY(LIME, Lime, 0x00, 0xFF, 0x00)                                 \
    __ENTRY(LIMEGREEN, LimeGreen, 0x32, 0xCD, 0x32)                       \
    __ENTRY(LINEN, Linen, 0xFA, 0xF0, 0xE6)                               \
    __ENTRY(MAGENTA, Magenta, 0xFF, 0x00, 0xFF)                           \
    __ENTRY(MAROON, Maroon, 0x80, 0x00, 0x00)                             \
    __ENTRY(MEDIUMAQUAMARINE, MediumAquaMarine, 0x66, 0xCD, 0xAA)         \
    __ENTRY(MEDIUMBLUE, MediumBlue, 0x00, 0x00, 0xCD)                     \
    __ENTRY(MEDIUMORCHID, MediumOrchid, 0xBA, 0x55, 0xD3)                 \
    __ENTRY(MEDIUMPURPLE, MediumPurple, 0x93, 0x70, 0xDB)                 \
    __ENTRY(MEDIUMSEAGREEN, MediumSeaGreen, 0x3C, 0xB3, 0x71)             \
    __ENTRY(MEDIUMSLATEBLUE, MediumSlateBlue, 0x7B, 0x68, 0xEE)           \
    __ENTRY(MEDIUMSPRINGGREEN, MediumSpringGreen, 0x00, 0xFA, 0x9A)       \
    __ENTRY(MEDIUMTURQUOISE, MediumTurquoise, 0x48, 0xD1, 0xCC)           \
    __ENTRY(MEDIUMVIOLETRED, MediumVioletRed, 0xC7, 0x15, 0x85)           \
    __ENTRY(MIDNIGHTBLUE, MidnightBlue, 0x19, 0x19, 0x70)                 \
    __ENTRY(MINTCREAM, MintCream, 0xF5, 0xFF, 0xFA)                       \
    __ENTRY(MISTYROSE, MistyRose, 0xFF, 0xE4, 0xE1)                       \
    __ENTRY(MOCCASIN, Moccasin, 0xFF, 0xE4, 0xB5)                         \
    __ENTRY(NAVAJOWHITE, NavajoWhite, 0xFF, 0xDE, 0xAD)                   \
    __ENTRY(NAVY, Navy, 0x00, 0x00, 0x80)                                 \
    __ENTRY(OLDLACE, OldLace, 0xFD, 0xF5, 0xE6)                           \
    __ENTRY(OLIVE, Olive, 0x80, 0x80, 0x00)                               \
    __ENTRY(OLIVEDRAB, OliveDrab, 0x6B, 0x8E, 0x23)                       \
    __ENTRY(ORANGE, Orange, 0xFF, 0xA5, 0x00)                             \
    __ENTRY(ORANGERED, OrangeRed, 0xFF, 0x45, 0x00)                       \
    __ENTRY(ORCHID, Orchid, 0xDA, 0x70, 0xD6)                             \
    __ENTRY(PALEGOLDENROD, PaleGoldenRod, 0xEE, 0xE8, 0xAA)               \
    __ENTRY(PALEGREEN, PaleGreen, 0x98, 0xFB, 0x98)                       \
    __ENTRY(PALETURQUOISE, PaleTurquoise, 0xAF, 0xEE, 0xEE)               \
    __ENTRY(PALEVIOLETRED, PaleVioletRed, 0xDB, 0x70, 0x93)               \
    __ENTRY(PAPAYAWHIP, PapayaWhip, 0xFF, 0xEF, 0xD5)                     \
    __ENTRY(PEACHPUFF, PeachPuff, 0xFF, 0xDA, 0xB9)                       \
    __ENTRY(PERU, Peru, 0xCD, 0x85, 0x3F)                                 \
    __ENTRY(PINK, Pink, 0xFF, 0xC0, 0xCB)                                 \
    __ENTRY(PLUM, Plum, 0xDD, 0xA0, 0xDD)                                 \
    __ENTRY(POWDERBLUE, PowderBlue, 0xB0, 0xE0, 0xE6)                     \
    __ENTRY(PURPLE, Purple, 0x80, 0x00, 0x80)                             \
    __ENTRY(REBECCAPURPLE, RebeccaPurple, 0x66, 0x33, 0x99)               \
    __ENTRY(RED, Red, 0xFF, 0x00, 0x00)                                   \
    __ENTRY(ROSYBROWN, RosyBrown, 0xBC, 0x8F, 0x8F)                       \
    __ENTRY(ROYALBLUE, RoyalBlue, 0x41, 0x69, 0xE1)                       \
    __ENTRY(SADDLEBROWN, SaddleBrown, 0x8B, 0x45, 0x13)                   \
    __ENTRY(SALMON, Salmon, 0xFA, 0x80, 0x72)                             \
    __ENTRY(SANDYBROWN, SandyBrown, 0xF4, 0xA4, 0x60)                     \
    __ENTRY(SEAGREEN, SeaGreen, 0x2E, 0x8B, 0x57)                         \
    __ENTRY(SEASHELL, SeaShell, 0xFF, 0xF5, 0xEE)                         \
    __ENTRY(SIENNA, Sienna, 0xA0, 0x52, 0x2D)                             \
    __ENTRY(SILVER, Silver, 0xC0, 0xC0, 0xC0)                             \
    __ENTRY(SKYBLUE, SkyBlue, 0x87, 0xCE, 0xEB)                           \
    __ENTRY(SLATEBLUE, SlateBlue, 0x6A, 0x5A, 0xCD)                       \
    __ENTRY(SLATEGRAY, SlateGray, 0x70, 0x80, 0x90)                       \
    __ENTRY(SLATEGREY, SlateGrey, 0x70, 0x80, 0x90)                       \
    __ENTRY(SNOW, Snow, 0xFF, 0xFA, 0xFA)                                 \
    __ENTRY(SPRINGGREEN, SpringGreen, 0x00, 0xFF, 0x7F)                   \
    __ENTRY(STEELBLUE, SteelBlue, 0x46, 0x82, 0xB4)                       \
    __ENTRY(TAN, Tan, 0xD2, 0xB4, 0x8C)                                   \
    __ENTRY(TEAL, Teal, 0x00, 0x80, 0x80)                                 \
    __ENTRY(THISTLE, Thistle, 0xD8, 0xBF, 0xD8)                           \
    __ENTRY(TOMATO, Tomato, 0xFF, 0x63, 0x47)                             \
    __ENTRY(TURQUOISE, Turquoise, 0x40, 0xE0, 0xD0)                       \
    __ENTRY(VIOLET, Violet, 0xEE, 0x82, 0xEE)                             \
    __ENTRY(WHEAT, Wheat, 0xF5, 0xDE, 0xB3)                               \
    __ENTRY(WHITE, White, 0xFF, 0xFF, 0xFF)                               \
    __ENTRY(WHITESMOKE, WhiteSmoke, 0xF5, 0xF5, 0xF5)                     \
    __ENTRY(YELLOW, Yellow, 0xFF, 0xFF, 0x00)                             \
    __ENTRY(YELLOWGREEN, YellowGreen, 0x9A, 0xCD, 0x32)

#define COLOR_ENUM_ENTRY(__name_maj, __name_lower, __r, __g, __b) static const Color COLOR_##__name_maj = (Color){{__r, __g, __b, 255}};
COLOR_ENUM(COLOR_ENUM_ENTRY)
