#pragma once

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/lock.h>

#define CONSOLE_CELL_HEIGHT 16
#define CONSOLE_CELL_WIDTH 8 

typedef enum
{
    CSLC_DEFAULT_FORGROUND,
    CSLC_DEFAULT_BACKGROUND,
    
    CSLC_BLACK,
    CSLC_RED,
    CSLC_GREEN,
    CSLC_YELLOW,
    CSLC_BLUE,
    CSLC_MAGENTA,
    CSLC_CYAN,
    CSLC_LIGHT_GREY,

    CSLC_DARK_GREY,
    CSLC_LIGHT_RED,
    CSLC_LIGHT_GREEN,
    CSLC_LIGHT_YELLOW,
    CSLC_LIGHT_BLUE,
    CSLC_LIGHT_MAGENTA,
    CSLC_LIGHT_CYAN,
    CSLC_WHITE,

} console_color_t;

typedef enum
{
    CSSTATE_ESC,
    CSSTATE_BRACKET,
    CSSTATE_PARSE,
    CSSTATE_BGCOLOR,
    CSSTATE_FGCOLOR,
    CSSTATE_ENDVAL,
} console_state_t;

typedef struct
{
    char c;

    console_color_t fg;
    console_color_t bg;
} console_cell_t;

typedef struct
{
    list_t *cells;
} console_line_t;

typedef struct
{
    list_t *lines;
    console_line_t *current_line;

    console_state_t state;

    console_color_t fg;
    console_color_t bg;
    console_color_t newfg;
    console_color_t newbg;
} console_t;

void console_setup();

void console_print(const char *s);
void console_putchar(char c);

void console_read(const char *s, uint size);
void console_getchar(char c);