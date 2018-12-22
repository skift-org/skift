#include <skift/drawing.h>
#include <skift/logger.h>

#include "kernel/graphic.h"
#include "kernel/console.h"

int colors[] =
    {
        [CSLC_BLACK] = 0x1D1F21,
        [CSLC_RED] = 0xA54242,
        [CSLC_GREEN] = 0x8C9440,
        [CSLC_YELLOW] = 0xDE935F,
        [CSLC_BLUE] = 0x5F819D,
        [CSLC_MAGENTA] = 0x85678F,
        [CSLC_CYAN] = 0x5E8D87,
        [CSLC_LIGHT_GREY] = 0x707880,

        [CSLC_DARK_GREY] = 0x373B41,
        [CSLC_LIGHT_RED] = 0xCC6666,
        [CSLC_LIGHT_GREEN] = 0xB5BD68,
        [CSLC_LIGHT_YELLOW] = 0xF0C674,
        [CSLC_LIGHT_BLUE] = 0x81A2BE,
        [CSLC_LIGHT_MAGENTA] = 0xB294BB,
        [CSLC_LIGHT_CYAN] = 0x8ABEB7,
        [CSLC_WHITE] = 0xC5C8C6,

        [CSLC_DEFAULT_FORGROUND] = 0xC5C8C6,
        [CSLC_DEFAULT_BACKGROUND] = 0x1D1F21,
};

console_t console;
bitmap_t *framebuffer;

void console_setup()
{
    sk_lock_init(console.lock);

    console.state = CSSTATE_ESC;

    console.lines = list();
    console.current_line = NULL;

    console.bg = CSLC_DEFAULT_BACKGROUND;
    console.fg = CSLC_DEFAULT_FORGROUND;
    
    uint width, height;
    graphic_size(&width, &height);
    sk_log(LOG_DEBUG, "w: %d h: %d", width, height);
    framebuffer = bitmap(width, height);
}

void console_render()
{
    sk_log(LOG_DEBUG, "begin rendering");

    drawing_clear(framebuffer, colors[CSLC_DEFAULT_BACKGROUND]);
    drawing_line(framebuffer, 0,0, 800,66, colors[CSLC_RED]);

    int ll = 0;
    FOREACH(l, console.lines)
    {
        console_line_t *line = l->value;
        sk_log(LOG_DEBUG, "line %d", ll);

        int cc = 0;
        FOREACH(c, line->cells)
        {
            console_cell_t *cell = c->value;

            drawing_fillrect(framebuffer, cc * 8, framebuffer->height - ll * 16, 8, 16, colors[cell->bg]);
            drawing_char(framebuffer, cell->c, cc * 8, framebuffer->height - ll * 16, colors[cell->fg]);
            sk_log(LOG_DEBUG, "char %c", cell->c);
            cc++;
        }

        ll++;
    }

    graphic_blit(framebuffer->buffer);
}

console_cell_t *console_cell(char c)
{
    console_cell_t *cell = MALLOC(console_cell_t);

    cell->fg = console.fg;
    cell->bg = console.bg;

    cell->c = c;

    return cell;
}

console_line_t *console_newline()
{
    console_line_t *line = MALLOC(console_line_t);
    line->cells = list();

    sk_log(LOG_DEBUG, "Line %x created", line);

    return line;
}

void console_append_char(char c)
{
    if (console.current_line == NULL)
    {
        console.current_line = console_newline();
        list_push(console.lines, console.current_line);
    }

    if (c == '\n')
    {
        console.current_line = console_newline();
        list_push(console.lines, console.current_line);
    }
    else
    {
        sk_log(LOG_DEBUG, "writting char %c", c);
        list_pushback(console.current_line->cells, console_cell(c));
        sk_log(LOG_DEBUG, "writting char %c ok", c);
    }
}

void console_putchar_internal(char c)
{

    switch (console.state)
    {
    case CSSTATE_ESC:
        if (c == '\033')
        {
            console.newfg = console.fg;
            console.newbg = console.bg;

            console.state = CSSTATE_BRACKET;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    case CSSTATE_BRACKET:
        if (c == '[')
        {
            console.state = CSSTATE_PARSE;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    case CSSTATE_PARSE:
        if (c == '3')
        {
            console.state = CSSTATE_FGCOLOR;
        }
        else if (c == '4')
        {
            console.state = CSSTATE_BGCOLOR;
        }
        else if (c == '0')
        {
            // Reset colors
            console.newbg = CSLC_DEFAULT_BACKGROUND;
            console.newfg = CSLC_DEFAULT_FORGROUND;

            console.state = CSSTATE_ENDVAL;
        }
        else if (c == '1')
        {
            // Make it bright
            if (console.newfg < CSLC_DARK_GREY)
            {
                console.newfg += CSLC_DARK_GREY;
            }

            console.state = CSSTATE_ENDVAL;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    case CSSTATE_BGCOLOR:
        if (c >= '0' && c <= '7')
        {
            console.newbg = c - '0';
            console.state = CSSTATE_ENDVAL;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    case CSSTATE_FGCOLOR:
        if (c >= '0' && c <= '7')
        {
            if (console.newfg >= CSLC_DARK_GREY)
            {
                console.newfg = c - '0' + CSLC_DARK_GREY;
            }
            else
            {
                console.newfg = c - '0';
            }

            console.state = CSSTATE_ENDVAL;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    case CSSTATE_ENDVAL:
        if (c == ';')
        {
            console.state = CSSTATE_PARSE;
        }
        else if (c == 'm')
        {
            console.fg = console.newfg;
            console.bg = console.newbg;

            console.state = CSSTATE_ESC;
        }
        else
        {
            console.state = CSSTATE_ESC;
            console_append_char(c);
        }
        break;

    default:
        break;
    }
}

void console_print(const char *s)
{
    sk_log(LOG_DEBUG, "begin");

    LOCK(console.lock,
         {
             for (uint i = 0; s[i]; i++)
             {
                 console_append_char(s[i]);
                //console_putchar_internal(s[i]);
             }

             console_render();
         });

    sk_log(LOG_DEBUG, "end");
}

void console_putchar(char c)
{
    LOCK(console.lock,
         {
             console_putchar_internal(c);

             console_render();
         });
}

/* TODO: need full keyboad driver!
void console_read(const char * s, uint size)
{

}

void console_getchar(char c)
{

}
*/