#pragma once

#include <libsystem/unicode/UTF8Decoder.h>
#include <libterminal/Attributes.h>
#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>

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

    Terminal(int width, int height);

    ~Terminal();

    void clear(int fromx, int fromy, int tox, int toy);

    void clear_all();

    void clear_line(int line);

    void resize(int width, int height);

    TerminalCell cell_at(int x, int y);
    void cell_undirty(int x, int y);
    void set_cell(int x, int y, TerminalCell cell);
};

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
