#pragma once

#include <libsystem/io/Stream.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/RingBuffer.h>

class Lexer
{
private:
    Stream *_stream = nullptr;
    RingBuffer *_peek = nullptr;
    /*    or    */
    const char *_string = nullptr;

    size_t _size = 0;
    size_t _offset = 0;

public:
    Lexer(Stream *stream);

    Lexer(const char *string, size_t size);

    ~Lexer();

    bool ended();

    bool do_continue();

    void foreward();

    void foreward(size_t n);

    void foreward_codepoint();

    char peek(size_t peek);

    char current();

    Codepoint current_codepoint();

    bool current_is(const char *what);

    bool current_is_word(const char *word);

    void eat(const char *what);

    bool skip(char chr);

    bool skip_word(const char *word);

    // Read **JSON** escape sequence like \n \t \r or \uXXXX
    const char *read_escape_sequence();
};
