#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/unicode/UTF8Decoder.h>
#include <libterminal/Attributes.h>
#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>
#include <libterminal/Renderer.h>

struct Terminal;

typedef enum
{
    TERMINAL_STATE_WAIT_ESC,
    TERMINAL_STATE_EXPECT_BRACKET,
    TERMINAL_STATE_READ_ATTRIBUTE,
} TerminalState;

typedef struct
{
    int value;
    bool empty;
} TerminalParameter;

typedef struct Terminal
{
    int height;
    int width;
    TerminalCell *buffer;
    UTF8Decoder *decoder;
    TerminalRenderer *renderer;

    TerminalState state;
    TerminalCursor saved_cursor;
    TerminalCursor cursor;

    TerminalAttributes current_attributes;
    TerminalAttributes default_attributes;

#define TERMINAL_MAX_PARAMETERS 8
    int parameters_top;
    TerminalParameter parameters[TERMINAL_MAX_PARAMETERS];
} Terminal;

Terminal *terminal_create(int width, int height, TerminalRenderer *renderer);
void terminal_destroy(Terminal *terminal);

void terminal_clear(Terminal *terminal, int fromx, int fromy, int tox, int toy);
void terminal_clear_all(Terminal *terminal);
void terminal_clear_line(Terminal *terminal, int line);

TerminalCell terminal_cell_at(Terminal *terminal, int x, int y);
void terminal_cell_undirty(Terminal *terminal, int x, int y);
void terminal_set_cell(Terminal *terminal, int x, int y, TerminalCell cell);

void terminal_cursor_show(Terminal *terminal);
void terminal_cursor_hide(Terminal *terminal);
void terminal_cursor_move(Terminal *terminal, int offx, int offy);
void terminal_cursor_set(Terminal *terminal, int x, int y);

void terminal_scroll(Terminal *terminal, int how_many_line);
void terminal_new_line(Terminal *terminal);
void terminal_backspace(Terminal *terminal);

void terminal_append(Terminal *terminal, Codepoint codepoint);
void terminal_do_ansi(Terminal *terminal, Codepoint codepoint);

void terminal_write_codepoint(Terminal *terminal, Codepoint codepoint);
void terminal_write_char(Terminal *terminal, char c);
void terminal_write(Terminal *terminal, const char *buffer, size_t size);

void terminal_on_paint(Terminal *terminal, int x, int y, TerminalCell cell);
void terminal_on_cursor(Terminal *terminal, TerminalCursor cursor);
void terminal_on_blink(Terminal *terminal);

void terminal_repaint(Terminal *terminal);
void terminal_blink(Terminal *terminal);