
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
    if (codepoint == U'\n')
    {
        readline->should_continue = false;
    }
    else if (codepoint == U'\b')
    {
        if (readline->cursor > 0)
        {
            readline->cursor--;
            unicode_string_remove(readline->string, readline->cursor);
        }
    }
    else if (codepoint == U'\t')
    {
    }
    else if (codepoint == U'\e')
    {
    }
    else
    {
        unicode_string_insert(readline->string, codepoint, readline->cursor);
        readline->cursor++;
    }
}

Result readline_readline(ReadLine *readline, char **line)
{
    unicode_string_clear(readline->string);

    readline->cursor = 0;
    readline->old_cursor = 0;
    readline->should_continue = true;

    while (readline->should_continue)
    {
        char chr = stream_getchar(in_stream);

        if (handle_has_error(in_stream))
        {
            readline->should_continue = false;
            return handle_get_error(in_stream);
        }

        utf8decoder_write(readline->decoder, chr);

        readline_repaint(readline);
    }

    printf("\n\e[0m");

    *line = unicode_string_as_cstring(readline->string);

    return SUCCESS;
}

ReadLine *readline_create(void)
{
    ReadLine *readline = __create(ReadLine);

    readline->string = unicode_string_create(READLINE_ALLOCATED);
    readline->decoder = utf8decoder_create(readline, (UTF8DecoderCallback)readline_decode_callback);

    return readline;
}

void readline_destroy(ReadLine *readline)
{
    unicode_string_destroy(readline->string);
    utf8decoder_destroy(readline->decoder);

    free(readline);
}
