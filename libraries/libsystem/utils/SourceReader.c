#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/NumberParser.h>
#include <libsystem/utils/SourceReader.h>

SourceReader *source_create_from_stream(Stream *stream)
{
    SourceReader *source = __create(SourceReader);

    FileState state = {};
    stream_stat(stream, &state);

    source->stream = stream;
    source->size = state.size;
    source->peek = ringbuffer_create(SOURCE_READER_MAX_PEEK);

    return source;
}

SourceReader *source_create_from_string(const char *string, size_t size)
{
    SourceReader *source = __create(SourceReader);

    source->string = string;
    source->size = size;

    return source;
}

void source_destroy(SourceReader *source)
{
    if (source->peek)
    {
        ringbuffer_destroy(source->peek);
    }

    free(source);
}

bool source_ended(SourceReader *source)
{
    if (source->string)
    {
        return source->offset >= source->size;
    }
    else
    {
        return source->offset >= source->size || handle_has_error(source->stream);
    }
}

bool source_do_continue(SourceReader *source)
{
    return source->offset < source->size;
}

void source_foreward(SourceReader *source)
{
    source->offset++;

    if (source_ended(source))
    {
        return;
    }

    if (source->stream)
    {
        if (ringbuffer_is_empty(source->peek))
        {
            ringbuffer_putc(source->peek, stream_getchar(source->stream));
        }

        ringbuffer_getc(source->peek);
    }
}

char source_peek(SourceReader *source, size_t peek)
{
    assert(peek < SOURCE_READER_MAX_PEEK);

    size_t offset = source->offset + peek;

    if (offset >= source->size)
    {
        return '\0';
    }

    if (source->string)
    {
        return source->string[offset];
    }
    else
    {
        while (peek >= ringbuffer_used(source->peek))
        {
            ringbuffer_putc(source->peek, stream_getchar(source->stream));
        }

        return ringbuffer_peek(source->peek, peek);
    }
}

char source_current(SourceReader *source)
{
    return source_peek(source, 0);
}

bool source_current_is(SourceReader *source, const char *what)
{
    for (size_t i = 0; what[i]; i++)
    {
        if (source_current(source) == what[i])
        {
            return true;
        }
    }

    return false;
}

bool source_current_is_word(SourceReader *source, const char *word)
{
    for (size_t i = 0; word[i]; i++)
    {
        if (source_peek(source, i) != word[i])
        {
            return false;
        }
    }

    return true;
}

void source_eat(SourceReader *source, const char *what)
{
    while (source_current_is(source, what) &&
           source_do_continue(source))
    {
        source_foreward(source);
    }
}

bool source_skip(SourceReader *source, char chr)
{
    if (source_current(source) == chr)
    {
        source_foreward(source);

        return true;
    }

    return false;
}

bool source_skip_word(SourceReader *source, const char *word)
{
    if (source_current_is_word(source, word))
    {
        for (size_t i = 0; i < strlen(word); i++)
        {
            source_foreward(source);
        }

        return true;
    }

    return false;
}

#define XDIGITS "0123456789abcdef"

const char *source_read_escape_sequence(SourceReader *source)
{
    source_skip(source, '\\');
    char chr = source_current(source);

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
        char buffer[5];
        for (size_t i = 0; i < 4 && source_current_is(source, XDIGITS); i++)
        {
            buffer[i] = source_current(source);
            source_foreward(source);
        }

        static_assert(sizeof(Codepoint) == sizeof(unsigned int), "Expecting size of Codepoint being the same as unsigned int");

        Codepoint codepoint = 0;
        if (parse_uint(PARSER_HEXADECIMAL, buffer, 5, (unsigned int *)&codepoint))
        {
            static uint8_t utf8[5] = {};
            codepoint_to_utf8(codepoint, utf8);

            return (char *)utf8;
        }
        else
        {
            return "\\uXXXX";
        }
    }

    default:
        break;
    }

    static char buffer[3] = {'\\', 'x', '\0'};
    buffer[1] = chr;

    return buffer;
}
