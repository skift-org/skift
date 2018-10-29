#pragma once

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/ringbuffer.h>

typedef enum
{
    CONSOLE_COLOR_BLACK,
    CONSOLE_COLOR_BLUE,
    CONSOLE_COLOR_GREEN,
    CONSOLE_COLOR_CYAN,
    CONSOLE_COLOR_RED,
    CONSOLE_COLOR_MAGENTA,
    CONSOLE_COLOR_BROWN,
    CONSOLE_COLOR_GRAY,
} console_color_t;

typedef struct
{
    char c;
    console_color_t color;
} console_cell_t;

typedef struct
{
    uint width;
    uint height;

    uint cx;
    uint cy;

    console_cell_t *buffer;

} console_t;

console_t *console(uint width, uint height);
void console_delete(console_t *c);

void console_clear(console_t *c);
void console_scroll(console_t *c);
void console_writeln(console_t *c, const char *message);
uint console_readln(console_t *c, char *buffer, uint size);