#pragma once

#include <libsystem/unicode/UTF8Decoder.h>
#include <libterminal/Attributes.h>
#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>

namespace Terminal
{

enum class State
{
    WAIT_ESC,
    EXPECT_BRACKET,
    READ_ATTRIBUTE,
};

struct Parameter
{
    int value;
    bool empty;
};

struct Terminal
{
private:
    int _height;
    int _width;
    Cell *_buffer;
    UTF8Decoder _decoder;

    State _state = State::WAIT_ESC;
    Cursor _saved_cursor;
    Cursor _cursor;

    Attributes _attributes;

    static constexpr int MAX_PARAMETERS = 8;
    int _parameters_top;
    Parameter _parameters[MAX_PARAMETERS];

public:
    int width() { return _width; }

    int height() { return _height; }

    const Cursor &cursor() { return _cursor; }

    Terminal(int width, int height);

    ~Terminal();

    void clear(int fromx, int fromy, int tox, int toy);

    void clear_all();

    void clear_line(int line);

    void resize(int width, int height);

    Cell cell_at(int x, int y);

    void cell_undirty(int x, int y);

    void set_cell(int x, int y, Cell cell);

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

} // namespace Terminal
