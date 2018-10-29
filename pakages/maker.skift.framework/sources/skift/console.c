/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
#include <string.h>
#include <skift/console.h>

#define CONSOLE_SIZE(c) (sizeof(console_cell_t) * c->width * c->height)
#define CONSOLE_CELL(c, x, y) (c->buffer[(y) * c->width + (x)])

console_t* console(uint width, uint height)
{
    console_t * c = MALLOC(console_t);

    c->width = width;
    c->height = height;

    c->cx = 0;
    c->cy = 0;

    c->buffer = malloc(CONSOLE_SIZE(c));

    return c;
}

void console_delete(console_t* c)
{
    free(c->buffer);
    free(c);
}

void console_clear(console_t* c)
{
    memset(c->buffer, 0, CONSOLE_SIZE(c));
    
    c->cx = 0;
    c->cy = 0;
}

void console_scroll(console_t *c)
{
    for (u32 x = 0; x < c->width; x++)
    {
        for (u32 y = 0; y < c->height + 1; y++)
        {
            CONSOLE_CELL(c, x, y) = CONSOLE_CELL(c, x, y + 1);
        }
    }

    for (u32 i = 0; i < c->width; i++)
    {
        memset(&CONSOLE_CELL(c, i, c->height - 1), 0, sizeof(console_cell_t));
    }

    if (c->cy > 0)
    {
        c->cy--;
    }
}

void console_writeln(console_t* c, const char * message)
{
    for(uint i = 0; message[i]; i++)
    {
        char chr = message[i];
        CONSOLE_CELL(c, c->cx, c->cy).c = chr;
        
        c->cx++;
        if (c->cx >= c->width)
        {
            c->cx = 0;
            c->cy++;

            if (c->cy >= c->height)
            {
                console_scroll(c);
            }
        }
    }   
}

uint console_readln(console_t* c, char * buffer, uint size)
{
    UNUSED(c); UNUSED(buffer); UNUSED(size);

    return 0;
}