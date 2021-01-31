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

Result readline_readline(ReadLine *readline, char **line)
{
    unicode_string_clear(readline->string);

    readline->cursor = 0;
    readline->old_cursor = 0;
    readline->should_continue = true;

    while (readline->should_continue &&
           !readline->scan->ended())
    {
        if (readline->scan->current() == U'\n' ||
            readline->scan->current() == U'\r')
        {
            readline_recale_history(readline);

            readline->should_continue = false;

            readline->scan->foreward();
        }
        else if (readline->scan->current() == U'\b' ||
                 readline->scan->current() == 127 /*DEL*/)
        {
            readline_recale_history(readline);

            if (readline->cursor > 0)
            {
                readline->cursor--;
                unicode_string_remove(readline->string, readline->cursor);
            }

            readline->scan->foreward();
        }
        else if (readline->scan->current() == U'\t')
        {
            readline->scan->foreward();
        }
        else if (readline->scan->current() == U'\e')
        {
            readline->scan->foreward();

            if (readline->scan->current() != '[')
            {
                continue;
            }

            readline->scan->foreward();

            if (readline->scan->current() == 'A')
            {
                if (readline->history_current < history_length())
                {
                    readline->history_current++;
                    readline->cursor = unicode_string_length(readline_string(readline));
                }
            }
            else if (readline->scan->current() == 'B')
            {
                if (readline->history_current > 0)
                {
                    readline->history_current--;
                    readline->cursor = unicode_string_length(readline_string(readline));
                }
            }
            else if (readline->scan->current() == 'C')
            {
                if (readline->cursor < unicode_string_length(readline_string(readline)))
                {
                    readline->cursor++;
                }
            }
            else if (readline->scan->current() == 'D')
            {
                if (readline->cursor > 0)
                {
                    readline->cursor--;
                }
            }
            else if (readline->scan->current_is("0123456789"))
            {
                // https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences
                int digits = 0;

                while (readline->scan->current_is("0123456789"))
                {
                    digits *= 10;
                    digits += readline->scan->current() - '0';
                    readline->scan->foreward();
                }

                if (readline->scan->current() == '~')
                {
                    if (digits == 1) // Home
                    {
                        readline->cursor = 0;
                    }
                    else if (digits == 3) // Delete
                    {
                        if (readline->cursor < unicode_string_length(readline_string(readline)))
                        {
                            unicode_string_remove(readline->string, readline->cursor);
                        }
                    }
                    else if (digits == 4) // End
                    {
                        readline->cursor = unicode_string_length(readline_string(readline));
                    }
                }
            }

            readline->scan->foreward();
        }
        else
        {
            readline_recale_history(readline);

            readline->decoder->write(readline->scan->current());
            readline->scan->foreward();
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

    readline->decoder = new UTF8Decoder([readline](auto codepoint) {
        unicode_string_insert(readline->string, codepoint, readline->cursor);
        readline->cursor++;
    });

    readline->scan = new StreamScanner(readline->stream);

    return readline;
}

void readline_destroy(ReadLine *readline)
{
    delete readline->scan;
    delete readline->decoder;
    unicode_string_destroy(readline->string);

    free(readline);
}
