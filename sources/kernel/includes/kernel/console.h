#pragma once
#include "types.h"
#include "utils.h"

#define console_black 0x0
#define console_blue 0x1
#define console_green 0x2
#define console_cyan 0x3
#define console_red 0x4
#define console_magenta 0x5
#define console_brown 0x6
#define console_light_gray 0x7
#define console_gray 0x8
#define console_light_blue 0x9
#define console_light_green 0xA
#define console_light_cyan 0xB
#define console_light_red 0xC
#define console_light_magenta 0xD
#define console_light_yellow 0xE
#define console_white 0xF

#define console_defaul_fg 0x7;
#define console_defaul_bg 0x0;

typedef uchar console_color_t;

typedef struct
{
    console_color_t fg;
    console_color_t bg;
    upoint_t cursor;
} console_t;

extern bool console_bypass_lock;

void console_setup(void);
void console_color(console_color_t fg, console_color_t bg);
void console_put(char c);
void console_puts(string message);
void console_read(string buffer, int size);
void console_clear();