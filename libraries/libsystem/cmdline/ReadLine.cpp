#include <libsystem/Logger.h>
#include <libsystem/cmdline/History.h>
#include <libsystem/cmdline/ReadLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>

#define READLINE_ALLOCATED 128

static UnicodeString *readline_string(ReadLine *readline)
{
    if (readline->history_current == 0)
    {
        return readline->string;
    }
    else
    {
        return history_peek(readline->history_current - 1);
    }
}

static char *readline_cstring(ReadLine *readline)
{
    return unicode_string_as_cstring(readline_string(readline));
}

static void readline_recale_history(ReadLine *readline)
{
    if (readline->history_current != 0)
    {
        unicode_string_copy(history_peek(readline->history_current - 1), readline->string);
        readline->history_current = 0;
    }
}

static void readline_repaint(ReadLine *readline)
{
    printf("\e[%dD\e[J", readline->old_cursor);

    __cleanup_malloc char *cstring_buffer = readline_cstring(readline);

    stream_write(out_stream, cstring_buffer, strlen(cstring_buffer));

    printf("\e[%dD", unicode_string_length(readline_string(readline)));

    printf("\e[%dC", readline->cursor);

    readline->old_cursor = readline->cursor;
}

static void readline_decode_callback(ReadLine *readline, Codepoint codepoint)
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
           !readline->lexer->ended())
    {
        if (readline->lexer->current() == U'\n')
        {
            readline_recale_history(readline);

            readline->should_continue = false;

            readline->lexer->foreward();
        }
        else if (readline->lexer->current() == U'\b')
        {
            readline_recale_history(readline);

            if (readline->cursor > 0)
            {
                readline->cursor--;
                unicode_string_remove(readline->string, readline->cursor);
            }

            readline->lexer->foreward();
        }
        else if (readline->lexer->current() == U'\t')
        {
            readline->lexer->foreward();
        }
        else if (readline->lexer->current() == U'\e')
        {
            readline->lexer->foreward();

            if (readline->lexer->current() != '[')
            {
                continue;
            }

            readline->lexer->foreward();

            if (readline->lexer->current() == 'A')
            {
                if (readline->history_current < history_length())
                {
                    readline->history_current++;
                    readline->cursor = unicode_string_length(readline_string(readline));
                }
            }
            else if (readline->lexer->current() == 'B')
            {
                if (readline->history_current > 0)
                {
                    readline->history_current--;
                    readline->cursor = unicode_string_length(readline_string(readline));
                }
            }
            else if (readline->lexer->current() == 'C')
            {
                if (readline->cursor < unicode_string_length(readline_string(readline)))
                    readline->cursor++;
            }
            else if (readline->lexer->current() == 'D')
            {
                if (readline->cursor > 0)
                    readline->cursor--;
            }

            readline->lexer->foreward();
        }
        else
        {
            readline_recale_history(readline);

            utf8decoder_write(readline->decoder, readline->lexer->current());
            readline->lexer->foreward();
        }

        readline_repaint(readline);
    }

    printf("\n\e[0m");

    if (handle_has_error(readline->stream))
    {
        return handle_get_error(readline->stream);
    }

    *line = readline_cstring(readline);

    history_commit(readline->string);

    return SUCCESS;
}

ReadLine *readline_create(Stream *stream)
{
    ReadLine *readline = __create(ReadLine);

    readline->stream = stream;

    readline->string = unicode_string_create(READLINE_ALLOCATED);
    readline->decoder = utf8decoder_create(readline, (UTF8DecoderCallback)readline_decode_callback);
    readline->lexer = new Lexer(readline->stream);

    return readline;
}

void readline_destroy(ReadLine *readline)
{
    delete readline->lexer;
    utf8decoder_destroy(readline->decoder);
    unicode_string_destroy(readline->string);

    free(readline);
}
