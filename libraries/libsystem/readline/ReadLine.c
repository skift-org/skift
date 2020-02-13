/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/readline/ReadLine.h>

#define READLINE_ALLOCATED 128

ReadLine *readline_create(void)
{
    ReadLine *readline = __create(ReadLine);

    return readline;
}

void readline_destroy(ReadLine *readline)
{
    free(readline->buffer);
    free(readline);
}

void readline_repaint(ReadLine *readline)
{
    printf("\e[%dD\e[J", readline->old_cursor);

    stream_write(out_stream, readline->buffer, readline->used);

    printf("\e[%dD", readline->used);

    printf("\e[%dC", readline->cursor);

    readline->old_cursor = readline->used;
}

char *readline_readline(ReadLine *readline)
{
    readline->buffer = calloc(1, READLINE_ALLOCATED + 1);
    readline->allocated = READLINE_ALLOCATED;
    readline->buffer[0] = '\0';
    readline->cursor = 0;
    readline->old_cursor = 0;
    readline->used = 0;

    while (true)
    {
        char chr = stream_getchar(in_stream);

        if (chr == '\n')
        {
            printf("\n");
            break;
        }
        else if (chr == '\b')
        {
            if (readline->cursor > 0)
            {
                memcpy(readline->buffer + readline->cursor - 1, readline->buffer + readline->cursor, readline->used - readline->cursor);

                readline->cursor--;
                readline->used--;

                readline_repaint(readline);
            }
        }
        else if (chr != '\t')
        {
            if (readline->used + 1 >= readline->allocated)
            {
                readline->allocated *= 1.25;
                readline->buffer = realloc(readline->buffer, readline->allocated);
            }

            memcpy(readline->buffer + readline->cursor + 1,
                   readline->buffer + readline->cursor,
                   readline->used - readline->cursor);

            readline->buffer[readline->cursor] = chr;

            readline->cursor++;
            readline->used++;

            readline_repaint(readline);
        }
    }

    printf("\e[0m");

    readline->buffer[readline->used] = '\0';

    return readline->buffer;
}