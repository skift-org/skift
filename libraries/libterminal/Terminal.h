#pragma once

#include <libsystem/unicode/UTF8Decoder.h>
#include <libterminal/Attributes.h>
#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>

struct Terminal;

enum class TerminalState
{
    WAIT_ESC,
    EXPECT_BRACKET,
    READ_ATTRIBUTE,
};

struct TerminalParameter
{
    int value;
    bool empty;
};

struct Terminal
{
    int height;
    int width;
    TerminalCell *buffer;
    UTF8Decoder *decoder;

    TerminalState state;
    TerminalCursor saved_cursor;
    TerminalCursor cursor;

    TerminalAttributes current_attributes;
    TerminalAttributes default_attributes;

#define TERMINAL_MAX_PARAMETERS 8
    int parameters_top;
    TerminalParameter parameters[TERMINAL_MAX_PARAMETERS];
};

Terminal *terminal_create(int width, int height);
void terminal_destroy(Terminal *terminal);

void terminal_clear(Terminal *terminal, int fromx, int fromy, int tox, int toy);
void terminal_clear_all(Terminal *terminal);
void terminal_clear_line(Terminal *terminal, int line);

void terminal_resize(Terminal *terminal, int width, int height);

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
