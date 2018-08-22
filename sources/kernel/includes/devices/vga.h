#pragma once
#include "types.h"

#define vga_screen_width 80
#define vga_screen_height 25

#define vga_black         0x0
#define vga_blue          0x1
#define vga_green         0x2
#define vga_cyan          0x3
#define vga_red           0x4
#define vga_magenta       0x5
#define vga_brown         0x6
#define vga_light_gray    0x7

#define vga_gray          0x8
#define vga_light_blue    0x9
#define vga_light_green   0xA
#define vga_light_cyan    0xB
#define vga_light_red     0xC
#define vga_light_magenta 0xD
#define vga_light_yellow  0xE
#define vga_white         0xF

#define vga_frame_buffer 0xB8000
#define vga_color(fg, bg) (bg << 4 | fg)
#define vga_entry(c, fg, bg) ((bg << 4 | fg) << 8 | c)

typedef uchar vga_color_t;
typedef ushort vga_entry_t;

void vga_setup();
void vga_clear(vga_color_t fg, vga_color_t bg);
void vga_scroll(vga_color_t bg);
void vga_cell(u32 x, u32 y, vga_entry_t entry);
void vga_text(u32 x, u32 y, string text, vga_color_t fg, vga_color_t bg);
void vga_cursor(s32 x, s32 y);
void vga_hide_cursor();