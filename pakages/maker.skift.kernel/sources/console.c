
/*
 * TODO:
 *  - Support for \t, \b
 */

#include <math.h>
#include <string.h>

#include <skift/logger.h>
#include <skift/drawing.h>
#include <skift/atomic.h>

#include "kernel/graphic.h"
#include "kernel/console.h"

int colors[] =
    {
        [CSLC_DEFAULT_FORGROUND] = 0xC5C8C6,
        [CSLC_DEFAULT_BACKGROUND] = 0x1D1F21,

        [CSLC_BLACK] = 0x1D1F21, // 0
        [CSLC_RED] = 0xA54242,   // 1
        [CSLC_GREEN] = 0x8C9440, // 2
        [CSLC_YELLOW] = 0xDE935F, // 3
        [CSLC_BLUE] = 0x5F819D, // 4
        [CSLC_MAGENTA] = 0x85678F, // 5
        [CSLC_CYAN] = 0x5E8D87, // 6
        [CSLC_LIGHT_GREY] = 0x707880,  // 7

        [CSLC_DARK_GREY] = 0x373B41,
        [CSLC_LIGHT_RED] = 0xCC6666,
        [CSLC_LIGHT_GREEN] = 0xB5BD68,
        [CSLC_LIGHT_YELLOW] = 0xF0C674,
        [CSLC_LIGHT_BLUE] = 0x81A2BE,
        [CSLC_LIGHT_MAGENTA] = 0xB294BB,
        [CSLC_LIGHT_CYAN] = 0x8ABEB7,
        [CSLC_WHITE] = 0xC5C8C6,
};

console_t* console(uint w, uint h)
{
    console_t* c = MALLOC(console_t);

    c->cx = 0;
    c->cy = 0;
    c->w = w;
    c->h = h;

    c->screen = (console_cell_t*)malloc(sizeof(console_cell_t) * w * h);

    c->fg = CSLC_DEFAULT_FORGROUND;
    c->bg = CSLC_DEFAULT_BACKGROUND;

    return c;
}

console_cell_t* console_cell(console_t* c, uint x, uint y)
{
    if (x < c->w && y < c->h)
    {
        return &c->screen[x + y * c->w];
    }

    return NULL;
}

void console_scroll(console_t *c)
{
    memcpy(c->screen, (byte*)c->screen + (sizeof(console_cell_t) * c->w), (c->h - 1) * c->w * sizeof(console_cell_t));

    
    for(uint x = 0; x < c->w; x++)
    {
        console_cell_t *cell = console_cell(c, x, c->h-1);
        
        cell->c = ' ';
        cell->bg = CSLC_DEFAULT_BACKGROUND;
        cell->fg = CSLC_DEFAULT_FORGROUND;
    }
    
    c->cy--;
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

console_t* cons = NULL;
bitmap_t* console_framebuffer;

void console_setup()
{
    uint width, height = 0;
    graphic_size(&width, &height);
    cons = console(width / 8, height / 16);
    console_framebuffer = bitmap(width, height);;
}


void console_draw()
{
    drawing_clear(console_framebuffer, colors[CSLC_DEFAULT_BACKGROUND]);

    for(uint y = 0; y < cons->h; y++)
    {
        for(uint x = 0; x < cons->w; x++)
        {
            console_cell_t* cell = console_cell(cons, x, y);

            drawing_fillrect(console_framebuffer,x * 8, y * 16, 8, 16, colors[cell->bg]);
            drawing_char(console_framebuffer, cell->c, x * 8, y * 16, colors[cell->fg]);
        }
    }

    graphic_blit(console_framebuffer->buffer);
}

void console_append(char c)
{
    if (c == '\n')
    {
        console_newline(cons);
    }
    else
    {
        console_cell_t* cell = console_cell(cons, cons->cx, cons->cy);
        cell->c = c;
        cell->bg = cons->bg;
        cell->fg = cons->fg;

        cons->cx++;

        if (cons->cx >= cons->w) console_newline(cons);
    }
}

void console_process(char c)
{
    switch (cons->state)
     {
     case CSSTATE_ESC:
         if (c == '\033')
         {
             cons->newfg = cons->fg;
             cons->newbg = cons->bg;
             cons->state = CSSTATE_BRACKET;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     case CSSTATE_BRACKET:
         if (c == '[')
         {
             cons->state = CSSTATE_PARSE;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     case CSSTATE_PARSE:
         if (c == '3')
         {
             cons->state = CSSTATE_FGCOLOR;
         }
         else if (c == '4')
         {
             cons->state = CSSTATE_BGCOLOR;
         }
         else if (c == '0')
         {
             // Reset colors
             cons->newbg = CSLC_DEFAULT_BACKGROUND;
             cons->newfg = CSLC_DEFAULT_FORGROUND;
 
             cons->state = CSSTATE_ENDVAL;
         }
         else if (c == '1')
         {
             // Make it bright
             if (cons->newfg < CSLC_DARK_GREY)
             {
                 cons->newfg += 8;
             }
 
             cons->state = CSSTATE_ENDVAL;
         }
         else if (c== 'H')
         {
            cons->cx = 0;
            cons->cy = 0;

            cons->state = CSSTATE_ESC;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     case CSSTATE_BGCOLOR:
         if (c >= '0' && c <= '7')
         {
             cons->newbg = c - '0' + CSLC_BLACK;
             cons->state = CSSTATE_ENDVAL;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     case CSSTATE_FGCOLOR:
         if (c >= '0' && c <= '7')
         {
             if (cons->newfg >= CSLC_DARK_GREY)
             {
                 cons->newfg = c - '0' + CSLC_BLACK + 8;
             }
             else
             {
                 cons->newfg = c - '0' + CSLC_BLACK;
             }
 
             cons->state = CSSTATE_ENDVAL;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     case CSSTATE_ENDVAL:
         if (c == ';')
         {
             cons->state = CSSTATE_PARSE;
         }
         else if (c == 'm')
         {
             cons->fg = cons->newfg;
             cons->bg = cons->newbg;
 
             cons->state = CSSTATE_ESC;
         }
         else
         {
             cons->state = CSSTATE_ESC;
             console_append(c);
         }
         break;
 
     default:
         break;
     }
}

void console_print(const char *s)
{   
    sk_atomic_begin();

    if (cons != NULL)
    {
        for(uint i = 0; s[i]; i++)
        {
            console_process(s[i]);
        }

        console_draw();
    }

    sk_atomic_end();
}
