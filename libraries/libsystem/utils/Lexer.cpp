#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/Lexer.h>
#include <libsystem/utils/NumberParser.h>

#define SOURCE_READER_MAX_PEEK 16

Lexer::Lexer(Stream *stream)
{
    FileState state = {};
    stream_stat(stream, &state);

    _stream = stream;
    _size = state.size;
    _peek = ringbuffer_create(SOURCE_READER_MAX_PEEK);
}

Lexer::Lexer(const char *string, size_t size)
{
    _string = string;
    _size = size;
}

Lexer::~Lexer()
{
    if (_peek)
    {
        ringbuffer_destroy(_peek);
    }
}

bool Lexer::ended()
{
    if (_string)
    {
        return _offset >= _size;
    }
    else
    {
        return _offset >= _size || handle_has_error(_stream);
    }
}

bool Lexer::do_continue()
{
    return !ended();
}

void Lexer::foreward()
{
    _offset++;

    if (ended())
    {
        return;
    }

    if (_stream)
    {
        if (ringbuffer_is_empty(_peek))
        {
            ringbuffer_putc(_peek, stream_getchar(_stream));
        }

        ringbuffer_getc(_peek);
    }
}

char Lexer::peek(size_t peek)
{
    assert(peek < SOURCE_READER_MAX_PEEK);

    size_t offset = _offset + peek;

    if (offset >= _size)
    {
        return '\0';
    }

    if (_string)
    {
        return _string[offset];
    }
    else
    {
        while (peek >= ringbuffer_used(_peek))
        {
            ringbuffer_putc(_peek, stream_getchar(_stream));
        }

        return ringbuffer_peek(_peek, peek);
    }
}

char Lexer::current()
{
    return peek(0);
}

bool Lexer::current_is(const char *what)
{
    for (size_t i = 0; what[i]; i++)
    {
        if (current() == what[i])
        {
            return true;
        }
    }

    return false;
}

bool Lexer::current_is_word(const char *word)
{
    for (size_t i = 0; word[i]; i++)
    {
        if (peek(i) != word[i])
        {
            return false;
        }
    }

    return true;
}

void Lexer::eat(const char *what)
{
    while (Lexer::current_is(what) &&
           Lexer::do_continue())
    {
        foreward();
    }
}

bool Lexer::skip(char chr)
{
    if (current() == chr)
    {
        foreward();

        return true;
    }

    return false;
}

bool Lexer::skip_word(const char *word)
{
    if (current_is_word(word))
    {
        for (size_t i = 0; i < strlen(word); i++)
        {
            foreward();
        }

        return true;
    }

    return false;
}

#define XDIGITS "0123456789abcdef"

const char *Lexer::read_escape_sequence()
{
    skip('\\');

    if (ended())
    {
        return "\\";
    }

    char chr = current();
    foreward();

    switch (chr)
    {
    case '"':
        return "\"";

    case '\\':
        return "\\";

    case '/':
        return "/";

    case 'b':
        return "\b";

    case 'f':
        return "\f";

    case 'n':
        return "\n";

    case 'r':
        return "\r";

    case 't':
        return "\t";

    case 'u':
    {
        auto read_4hex = [this]() {
            char buffer[5];
            for (size_t i = 0; i < 4 && current_is(XDIGITS); i++)
            {
                buffer[i] = current();
                foreward();
            }

            uint value = 0;
            parse_uint(PARSER_HEXADECIMAL, buffer, 5, (unsigned int *)&value);
            return value;
        };

        uint first_surrogate = read_4hex();

        if (first_surrogate >= 0xDC00 && first_surrogate <= 0xDFFF)
        {
            return u8"�";
        }

        if (!(first_surrogate >= 0xD800 && first_surrogate <= 0xDBFF))
        {
            // Not an UTF16 surrogate pair.
            static uint8_t utf8[5] = {};
            codepoint_to_utf8((Codepoint)first_surrogate, utf8);
            return (char *)utf8;
        }

        if (!skip_word("\\u"))
        {
            return u8"�";
        }

        uint second_surrogate = read_4hex();

        if ((second_surrogate < 0xDC00) || (second_surrogate > 0xDFFF))
        {
            // Invalid second half of the surrogate pair
            return u8"�";
        }

        Codepoint codepoint = 0x10000 + (((first_surrogate & 0x3FF) << 10) | (second_surrogate & 0x3FF));

        static uint8_t utf8[5] = {};
        codepoint_to_utf8((Codepoint)codepoint, utf8);
        return (char *)utf8;
    }

    default:
        break;
    }

    static char buffer[3] = "\\x";
    buffer[1] = chr;

    return buffer;
}
