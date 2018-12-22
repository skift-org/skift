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

console_line_t* line();

console_t* console()
{
    console_t* c = MALLOC(console_t);

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

console_cell_t* cell(char c)
{
    console_cell_t* cell = MALLOC(console_cell_t);

    cell->c = c;

    return cell;
}

console_t* cons;
bitmap_t* framebuffer;

void console_setup()
{
    //cons = console();
    uint width, height = 0;
    graphic_size(&width, &height);
    framebuffer = bitmap(width, height);
    graphic_blit(framebuffer->buffer);

    sk_log(LOG_INFO, "console created at 0x0%8x with framebuffer at 0x%08x.", cons, framebuffer->buffer);
}

void console_draw()
{
    drawing_clear(framebuffer, colors[CSLC_DEFAULT_BACKGROUND]);

    int y = 0;
    FOREACH(l, cons->lines)
    {
        console_line_t* line = (console_line_t*)l->value;

        int x = 0;
        FOREACH(c, line->cells)
        {
            console_cell_t* cell = (console_cell_t*)c->value;

            drawing_char(framebuffer, cell->c, x * 8, y * 16, colors[CSLC_DEFAULT_FORGROUND]);
            x++;
        }

        y++;
    }

    graphic_blit(framebuffer->buffer);
}

void console_print(const char *s)
{   
    for(uint i = 0; s[i]; i++)
    {
        list_pushback(cons->current_line->cells, cell(s[i]));
    }
    
    console_draw();
}