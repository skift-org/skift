#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

#define VGA_FRAME_BUFFER 0XB8000

#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGHT 25

#define VGACOLOR_BLACK         0X0
#define VGACOLOR_BLUE          0X1
#define VGACOLOR_GREEN         0X2
#define VGACOLOR_CYAN          0X3
#define VGACOLOR_RED           0X4
#define VGACOLOR_MAGENTA       0X5
#define VGACOLOR_BROWN         0X6
#define VGACOLOR_LIGHT_GRAY    0X7
#define VGACOLOR_GRAY          0X8
#define VGACOLOR_LIGHT_BLUE    0X9
#define VGACOLOR_LIGHT_GREEN   0XA
#define VGACOLOR_LIGHT_CYAN    0XB
#define VGACOLOR_LIGHT_RED     0XC
#define VGACOLOR_LIGHT_MAGENTA 0XD
#define VGACOLOR_LIGHT_YELLOW  0XE
#define VGACOLOR_WHITE         0XF

#define VGA_COLOR(__fg, __bg) (__bg << 4 | __fg)
#define VGA_ENTRY(__c, __fg, __bg) ((((__bg) & 0XF) << 4 | ((__fg) & 0XF)) << 8 | ((__c) & 0XFF))

void vga_cell(u32 x, u32 y, ushort entry);
void vga_cursor(s32 x, s32 y);
void vga_hide_cursor();