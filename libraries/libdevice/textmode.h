#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define TEXTMODE_DEVICE "/dev/txt"

#define TEXTMODE_CALL_GET_INFO 0
#define TEXTMODE_CALL_SET_INFO 1
#define TEXTMODE_CALL_SET_CELL 2

#define TEXTMODE_COLOR_BLACK 0x0
#define TEXTMODE_COLOR_BLUE 0x1
#define TEXTMODE_COLOR_GREEN 0x2
#define TEXTMODE_COLOR_CYAN 0x3
#define TEXTMODE_COLOR_RED 0x4
#define TEXTMODE_COLOR_MAGENTA 0x5
#define TEXTMODE_COLOR_BROWN 0x6
#define TEXTMODE_COLOR_LIGHT_GRAY 0x7
#define TEXTMODE_COLOR_GRAY 0x8
#define TEXTMODE_COLOR_LIGHT_BLUE 0x9
#define TEXTMODE_COLOR_LIGHT_GREEN 0xA
#define TEXTMODE_COLOR_LIGHT_CYAN 0xB
#define TEXTMODE_COLOR_LIGHT_RED 0xC
#define TEXTMODE_COLOR_LIGHT_MAGENTA 0xD
#define TEXTMODE_COLOR_LIGHT_YELLOW 0xE
#define TEXTMODE_COLOR_WHITE 0xF

#define TEXTMODE_COLOR(__fg, __bg) (__bg << 4 | __fg)
#define TEXTMODE_ENTRY(__c, __fg, __bg) ((((__bg)&0XF) << 4 | ((__fg)&0XF)) << 8 | ((__c)&0XFF))

typedef struct
{
    int width;
    int height;
    int cursor_x;
    int cursor_y;
} textmode_info_t;

typedef struct
{
    int x;
    int y;
    uint8_t fg;
    uint8_t bg;
    int c;
} textmode_cell_info_t;
