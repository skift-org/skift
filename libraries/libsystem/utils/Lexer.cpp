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
    if (state.type != FILE_TYPE_REGULAR)
    {
        _is_stream = true;
    }

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
        return ((_offset >= _size) && !_is_stream) || handle_has_error(_stream);
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

void Lexer::foreward(size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        foreward();
    }
}

void Lexer::foreward_codepoint()
{
    if ((current() & 0xf8) == 0xf0)
    {
        foreward(4);
    }
    else if ((current() & 0xf0) == 0xe0)
    {
        foreward(3);
    }
    else if ((current() & 0xe0) == 0xc0)
    {
        foreward(2);
    }
    else
    {
        foreward(1);
    }
}

char Lexer::peek(size_t peek)
{
    assert(peek < SOURCE_READER_MAX_PEEK);

    size_t offset = _offset + peek;

    if (offset >= _size && !_is_stream)
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

Codepoint Lexer::current_codepoint()
{
    size_t size = 0;
    Codepoint codepoint = peek(0);

    if ((current() & 0xf8) == 0xf0)
    {
        size = 4;
        codepoint = (0x07 & codepoint) << 18;
    }
    else if ((current() & 0xf0) == 0xe0)
    {
        size = 3;
        codepoint = (0x0f & codepoint) << 12;
    }
    else if ((current() & 0xe0) == 0xc0)
    {
        codepoint = (0x1f & codepoint) << 6;
        size = 2;
    }

    for (size_t i = 1; i < size; i++)
    {
        codepoint |= (0x3f & peek(i)) << (6 * (size - i - 1));
    }

    return codepoint;
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
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
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
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        uint second_surrogate = read_4hex();

        if ((second_surrogate < 0xDC00) || (second_surrogate > 0xDFFF))
        {
            // Invalid second half of the surrogate pair
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
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
