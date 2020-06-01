
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/cmdline/ReadLine.h>
#include <libsystem/io/Stream.h>

#define READLINE_ALLOCATED 128

void readline_repaint(ReadLine *readline)
{
    printf("\e[%dD\e[J", readline->old_cursor);

    __cleanup_malloc char *cstring_buffer = unicode_string_as_cstring(readline->string);

    stream_write(out_stream, cstring_buffer, strlen(cstring_buffer));

    printf("\e[%dD", unicode_string_length(readline->string));

    printf("\e[%dC", readline->cursor);

    readline->old_cursor = readline->cursor;
}

void readline_decode_callback(ReadLine *readline, Codepoint codepoint)
{
    unicode_string_insert(readline->string, codepoint, readline->cursor);
    readline->cursor++;
}

Result readline_readline(ReadLine *readline, char **line)
{
    unicode_string_clear(readline->string);

    readline->cursor = 0;
    readline->old_cursor = 0;
    readline->should_continue = true;

    while (readline->should_continue &&
           !source_ended(readline->reader))
    {
        if (source_current(readline->reader) == U'\n')
        {
            readline->should_continue = false;
            source_foreward(readline->reader);
        }
        else if (source_current(readline->reader) == U'\b')
        {
            if (readline->cursor > 0)
            {
                readline->cursor--;
                unicode_string_remove(readline->string, readline->cursor);
            }

            source_foreward(readline->reader);
        }
        else if (source_current(readline->reader) == U'\t')
        {
            source_foreward(readline->reader);
        }
        else if (source_current(readline->reader) == U'\e')
        {
            source_foreward(readline->reader);

            if (source_current(readline->reader) != '[')
            {
                continue;
            }

            source_foreward(readline->reader);

            if (source_current(readline->reader) == 'C')
            {
                if (readline->cursor < unicode_string_length(readline->string))
                    readline->cursor++;
            }
            else if (source_current(readline->reader) == 'D')
            {
                if (readline->cursor > 0)
                    readline->cursor--;
            }

            source_foreward(readline->reader);
        }
        else
        {
            utf8decoder_write(readline->decoder, source_current(readline->reader));
            source_foreward(readline->reader);
        }

        readline_repaint(readline);
    }

    printf("\n\e[0m");

    *line = unicode_string_as_cstring(readline->string);

    return handle_get_error(readline->stream);
}

ReadLine *readline_create(Stream *stream)
{
    ReadLine *readline = __create(ReadLine);

    readline->stream = stream;

    readline->string = unicode_string_create(READLINE_ALLOCATED);
    readline->decoder = utf8decoder_create(readline, (UTF8DecoderCallback)readline_decode_callback);
    readline->reader = source_create_from_stream(readline->stream);

    return readline;
}

void readline_destroy(ReadLine *readline)
{
    source_destroy(readline->reader);
    utf8decoder_destroy(readline->decoder);
    unicode_string_destroy(readline->string);

    free(readline);
}
