/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/*
 * TODO:
 *  - Support for \t, \b
 *  - Support for cursor movements: https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_sequences
 *  - Support for true color
 *      ESC[ 38;2;<r>;<g>;<b>m Select RGB foreground color
 *      ESC[ 48;2;<r>;<g>;<b>m Select RGB background color
 */

#include <ctype.h>
#include <math.h>
#include <string.h>

#include <skift/logger.h>
#include <skift/drawing.h>
#include <skift/atomic.h>

#include "kernel/filesystem.h"
#include "kernel/graphic.h"
#include "kernel/console.h"

console_t *cons = NULL;
bitmap_t *console_framebuffer;

int colors[] =
    {
        [CCOLOR_DEFAULT_BACKGROUND] = 0x1D1F21,
        [CCOLOR_DEFAULT_FORGROUND] = 0xC5C8C6,

        [CCOLOR_BLACK] = 0x1D1F21,      // 0
        [CCOLOR_RED] = 0xA54242,        // 1
        [CCOLOR_GREEN] = 0x8C9440,      // 2
        [CCOLOR_YELLOW] = 0xDE935F,     // 3
        [CCOLOR_BLUE] = 0x5F819D,       // 4
        [CCOLOR_MAGENTA] = 0x85678F,    // 5
        [CCOLOR_CYAN] = 0x5E8D87,       // 6
        [CCOLOR_LIGHT_GREY] = 0x707880, // 7

        [CCOLOR_DARK_GREY] = 0x373B41,
        [CCOLOR_LIGHT_RED] = 0xCC6666,
        [CCOLOR_LIGHT_GREEN] = 0xB5BD68,
        [CCOLOR_LIGHT_YELLOW] = 0xF0C674,
        [CCOLOR_LIGHT_BLUE] = 0x81A2BE,
        [CCOLOR_LIGHT_MAGENTA] = 0xB294BB,
        [CCOLOR_LIGHT_CYAN] = 0x8ABEB7,
        [CCOLOR_WHITE] = 0xC5C8C6,
};

console_t *console(uint w, uint h)
{
    console_t *c = MALLOC(console_t);

    c->cx = 0;
    c->cy = 0;
    c->w = w;
    c->h = h;

    c->screen = (console_cell_t *)malloc(sizeof(console_cell_t) * w * h);

    c->fg = CCOLOR_DEFAULT_FORGROUND;
    c->bg = CCOLOR_DEFAULT_BACKGROUND;
    c->state = CSTATE_ESC;

    c->attr_sel = 0;

    return c;
}

console_cell_t *console_cell(console_t *c, uint x, uint y)
{
    if (x < c->w && y < c->h)
    {
        return &c->screen[x + y * c->w];
    }

    return NULL;
}

void console_draw_cell(console_t *c, uint x, uint y)
{
    console_cell_t *cell = console_cell(c, x, y);

    if (cell != NULL)
    {
        // blit the char to the screen
        drawing_fillrect(console_framebuffer, x * 8, y * 16, 8, 16, colors[cell->bg]);
        if (cell->c != ' ') drawing_char(console_framebuffer, cell->c, x * 8, y * 16, colors[cell->fg]);
        graphic_blit_region(console_framebuffer->buffer, x * 8, y * 16, 8, 16);
    }
}

void console_draw(console_t *c)
{
    for(uint y = 0; y < c->h; y++)
    {
        for(uint x = 0; x < c->w; x++)
        {
            console_draw_cell(c, x, y);
        }   
    }
}

void console_scroll(console_t *c)
{
    memcpy(c->screen, (byte *)c->screen + (sizeof(console_cell_t) * c->w), (c->h - 1) * c->w * sizeof(console_cell_t));

    for (uint x = 0; x < c->w; x++)
    {
        console_cell_t *cell = console_cell(c, x, c->h - 1);

        cell->c = ' ';
        cell->bg = CCOLOR_DEFAULT_BACKGROUND;
        cell->fg = CCOLOR_DEFAULT_FORGROUND;
    }

    c->cy--;

    console_draw(c);
}

void console_newline(console_t *c)
{
    c->cx = 0;
    c->cy++;

    if (c->cy >= c->h)
    {
        console_scroll(c);
    }
}

void console_clear(console_t *c)
{
    for (uint y = 0; y < c->h; y++)
    {
        for (uint x = 0; x < c->w; x++)
        {
            console_cell_t *cell = console_cell(c, x, y);
            cell->bg = CCOLOR_DEFAULT_BACKGROUND;
            cell->fg = CCOLOR_DEFAULT_FORGROUND;
            cell->c = ' ';

            console_draw_cell(cons, x, y);
        }
    }
}

void console_setup()
{
    uint width, height = 0;
    graphic_size(&width, &height);
    cons = console(width / 8, height / 16);
    console_framebuffer = bitmap(width, height);
    console_clear(cons);

    filesystem_mkdev("/dev/tty", (device_t){0});
}

void console_append(char c)
{
    if (c == '\n')
    {
        console_newline(cons);
    }
    else if (c == '\t')
    {
        for(uint i = 0; i < 8; i++)
        {
            console_append(' ');
        }
    }
    else
    {
        console_cell_t *cell = console_cell(cons, cons->cx, cons->cy);
        cell->c = c;
        cell->bg = cons->bg;
        cell->fg = cons->fg;

        console_draw_cell(cons, cons->cx, cons->cy);

        cons->cx++;

        if (cons->cx >= cons->w)
            console_newline(cons);
    }
}

void console_process(char c)
{
    switch (cons->state)
    {
    case CSTATE_ESC:
        if (c == '\033')
        {
            cons->state = CSTATE_BRACKET;

            cons->attr_sel = 0;
            cons->attr_stack[0] = 0;
        }
        else
        {
            cons->state = CSTATE_ESC;
            console_append(c);
        }
        break;

    case CSTATE_BRACKET:
        if (c == '[')
        {
            cons->state = CSTATE_ATTR;
        }
        else
        {
            cons->state = CSTATE_ESC;
            console_append(c);
        }
        break;
    case CSTATE_ATTR:
        if (isdigit(c))
        {
            cons->attr_stack[cons->attr_sel] *= 10;
            cons->attr_stack[cons->attr_sel] += (c - '0');
        }
        else
        {
            if ((cons->attr_sel) < CONSOLE_MAX_ATTR + 1)
            {
                cons->attr_sel++;
            }

            cons->attr_stack[cons->attr_sel] = 0;
            cons->state = CSTATE_ENDVAL;
        }
        break;
    default:
        break;
    }

    if (cons->state == CSTATE_ENDVAL)
    {
        if (c == ';')
        {
            cons->state = CSTATE_ATTR;
        }
        else
        {
            if (c == 'm')
            {
                for (uint i = 0; i < cons->attr_sel; i++)
                {
                    uint attr = cons->attr_stack[i];

                    if (attr >= 30 && attr <= 37)
                    {
                        // Set the forground color
                        if (cons->fg >= CCOLOR_DARK_GREY)
                        {
                            cons->fg = attr - 30 + CCOLOR_BLACK + 8;
                        }
                        else
                        {
                            cons->fg = attr - 30 + CCOLOR_BLACK;
                        }
                    }
                    else if (attr >= 40 && attr <= 47)
                    {
                        // Set the background color
                        cons->bg = attr - 40 + CCOLOR_BLACK;
                    }
                    else if (attr == 1)
                    {
                        // Make the color bright
                        if (cons->fg < CCOLOR_DARK_GREY)
                            cons->fg += 8;
                    }
                    else if (attr == 0)
                    {
                        // reset all display attributes
                        cons->bg = CCOLOR_DEFAULT_BACKGROUND;
                        cons->fg = CCOLOR_DEFAULT_FORGROUND;
                    }
                }
            }
            else if (c == 'H')
            {
                if (cons->attr_sel - 1 == 0)
                {
                    cons->cx = 0;
                    cons->cy = 0;
                }

                for (uint i = 0; i < cons->attr_sel; i++)
                {
                    /* code */
                }
            }
            else if (c == 'J')
            {
                console_clear(cons);
            }

            cons->state = CSTATE_ESC;
        }
    }
}

void console_print(const char *s)
{
    sk_atomic_begin();

    if (cons != NULL)
    {
        for (uint i = 0; s[i]; i++)
        {
            console_process(s[i]);
        }
    }

    sk_atomic_end();
}

void console_putchar(char c)
{
    sk_atomic_begin();

    if (cons != NULL)
    {
        console_process(c);
    }

    sk_atomic_end();
}
