
/*
 * TODO:
 *  - Support for \t, \b
 */

#include <math.h>

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

console_line_t* line();

console_t* console()
{
    console_t* c = MALLOC(console_t);

    c->fg = CSLC_DEFAULT_FORGROUND;
    c->bg = CSLC_DEFAULT_BACKGROUND;
    c->lines = list();
    c->current_line = line();
    list_pushback(c->lines, c->current_line);

    return c;
}

console_line_t* line()
{
    console_line_t* line = MALLOC(console_line_t);

    line->cells = list();

    return line;
}

console_cell_t* cell(char c, console_color_t fg, console_color_t bg)
{
    console_cell_t* cell = MALLOC(console_cell_t);

    cell->c = c;

    cell->fg = fg;
    cell->bg = bg;

    return cell;
}

console_t* cons = NULL;
bitmap_t* console_framebuffer;

void console_setup()
{
    cons = console();
    uint width, height = 0;
    graphic_size(&width, &height);
    console_framebuffer = bitmap(width, height);;
}


void console_draw()
{
    drawing_clear(console_framebuffer, colors[CSLC_DEFAULT_BACKGROUND]);

    int screen_height = console_framebuffer->height / CONSOLE_CELL_HEIGHT;
    int screen_width = console_framebuffer->width / CONSOLE_CELL_WIDTH;
    int screen_origine = min(screen_height, cons->lines->count);

    int y = 0;
    FOREACHR(l, cons->lines)
    {
        console_line_t* line = (console_line_t*)l->value;

        int x = 0;
        FOREACH(c, line->cells)
        {
            console_cell_t* cell = (console_cell_t*)c->value;

            drawing_fillrect(console_framebuffer,x * 8, (screen_origine - y) * 16, 8, 16, colors[cell->bg]);
            drawing_char(console_framebuffer, cell->c, x * 8, (screen_origine - y) * 16, colors[cell->fg]);
            
            if (x++>screen_width) break;
        }

        if (y++>screen_height) break;
    }

    graphic_blit(console_framebuffer->buffer);
}

void console_append(char c)
{
    if (c == '\n')
    {
        cons->current_line = line();
        list_pushback(cons->lines, cons->current_line);
    }
    else
    {
        list_pushback(cons->current_line->cells, cell(c, cons->fg, cons->bg));
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