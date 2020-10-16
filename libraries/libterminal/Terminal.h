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
private:
    int _height;
    int _width;
    TerminalCell *_buffer;
    UTF8Decoder _decoder;

    TerminalState _state;
    TerminalCursor _saved_cursor;
    TerminalCursor _cursor;

    TerminalAttributes _attributes;

    static constexpr int MAX_PARAMETERS = 8;
    int _parameters_top;
    TerminalParameter _parameters[MAX_PARAMETERS];

public:
    int width() { return _width; }

    int height() { return _height; }

    const TerminalCursor &cursor() { return _cursor; }

    Terminal(int width, int height);

    ~Terminal();

    void clear(int fromx, int fromy, int tox, int toy);

    void clear_all();

    void clear_line(int line);

    void resize(int width, int height);

    TerminalCell cell_at(int x, int y);

    void cell_undirty(int x, int y);

    void set_cell(int x, int y, TerminalCell cell);

    void cursor_move(int offx, int offy);

    void cursor_set(int x, int y);

    void scroll(int how_many_line);

    void new_line();

    void backspace();

    void append(Codepoint codepoint);

    void do_ansi(Codepoint codepoint);

    void write(Codepoint codepoint);

    void write(char c);

    void write(const char *buffer, size_t size);
};
