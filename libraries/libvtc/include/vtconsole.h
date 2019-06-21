#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

#define VTC_DEFAULT_FOREGROUND VTCOLOR_GREY
#define VTC_DEFAULT_BACKGROUND VTCOLOR_BLACK
#define VTC_DEFAULT_ATTR \
    (vtattr_t) { false, VTC_DEFAULT_FOREGROUND, VTC_DEFAULT_BACKGROUND }
#define VTC_ANSI_PARSER_STACK_SIZE 8

struct vtconsole;

typedef enum
{
    VTCOLOR_BLACK,
    VTCOLOR_RED,
    VTCOLOR_GREEN,
    VTCOLOR_YELLOW,
    VTCOLOR_BLUE,
    VTCOLOR_MAGENTA,
    VTCOLOR_CYAN,
    VTCOLOR_GREY,
} vtcolor_t;

typedef enum
{
    VTSTATE_ESC,
    VTSTATE_BRACKET,
    VTSTATE_ATTR,
    VTSTATE_ENDVAL,
} vtansi_parser_state_t;

typedef struct
{
    int value;
    bool empty;
} vtansi_arg_t;

typedef struct
{
    vtansi_parser_state_t state;
    vtansi_arg_t stack[VTC_ANSI_PARSER_STACK_SIZE];
    int index;
} vtansi_parser_t;

typedef struct
{
    bool bright;
    vtcolor_t fg;
    vtcolor_t bg;
} vtattr_t;

typedef struct
{
    char c;
    vtattr_t attr;
} vtcell_t;

typedef struct
{
    int x;
    int y;
} vtcursor_t;

typedef void (*vtc_paint_handler_t)(struct vtconsole *vtc, vtcell_t *cell, int x, int y);
typedef void (*vtc_cursor_handler_t)(struct vtconsole *vtc, vtcursor_t *cur);

typedef struct vtconsole
{

    int width;
    int height;

    vtattr_t attr;
    vtansi_parser_t ansiparser;

    vtcell_t *buffer;
    vtcursor_t cursor;

    vtc_paint_handler_t on_paint;
    vtc_cursor_handler_t on_move;
} vtconsole_t;

vtconsole_t *vtconsole(int width, int height, vtc_paint_handler_t on_paint, vtc_cursor_handler_t on_move);
void vtconsole_delete(vtconsole_t *c);

void vtconsole_clear(vtconsole_t *vtc, int fromx, int fromy, int tox, int toy);
void vtconsole_scroll(vtconsole_t *vtc);
void vtconsole_newline(vtconsole_t *vtc);

void vtconsole_putchar(vtconsole_t *vtc, char c);
void vtconsole_write(vtconsole_t *vtc, const char *buffer, uint size);