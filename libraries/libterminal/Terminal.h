#pragma once

#include <libsystem/unicode/UTF8Decoder.h>
#include <libterminal/Attributes.h>
#include <libterminal/Buffer.h>
#include <libterminal/Cursor.h>
#include <libterminal/Surface.h>

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
    Surface _surface;

    UTF8Decoder _decoder;

    State _state = State::WAIT_ESC;
    Cursor _saved_cursor;
    Cursor _cursor;

    Attributes _attributes;

    static constexpr int MAX_PARAMETERS = 8;
    int _parameters_top;
    Parameter _parameters[MAX_PARAMETERS];

public:
    int width() { return _surface.width(); }

    int height() { return _surface.height(); }

    Surface &surface() { return _surface; }

    Cursor &cursor() { return _cursor; }

    Terminal(int width, int height);

    void resize(int width, int height);

    void cursor_move(int offx, int offy);

    void cursor_set(int x, int y);

    void new_line();

    void backspace();

    void append(Codepoint codepoint);

    void do_ansi(Codepoint codepoint);

    void write(Codepoint codepoint);

    void write(char c);

    void write(const char *buffer, size_t size);
};

} // namespace Terminal
