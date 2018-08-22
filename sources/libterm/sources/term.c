#include <string.h>
#include <stdlib.h>
#include "libterm/term.h"
#include "libterm/ansi.h"
#include "utils.h"

term_t *term_alloc(uint width, uint height)
{
    term_t *term = MALLOC(term_t);

    lock_init(&term->lock, "term");

    term->cursor.line = 0;
    term->cursor.column = 0;

    term->width = width;
    term->height = height;

    term->screen = malloc(sizeof(termcell_t *) * height);
    for (size_t line = 0; line < height; line++)
    {
        term->screen[line] = calloc(width, sizeof(termcell_t));
    }

    return term;
}

void term_free(term_t *term)
{
    for (size_t x = 0; x < term->width; x++)
    {
        free(term->screen[x]);
    }

    free(term->screen);

    free(term);
}

void term_scroll(term_t *term)
{
    for (size_t line = 0; line < term->height - 1; line++)
    {
        memcpy(&term->screen[line], &term->screen[line + 1], sizeof(termcell_t) * term->width);
    }

    memset(term->screen[term->height - 1], 0, sizeof(termcell_t) * term->width);
}

void term_cell(term_t *term, uint line, uint column, char c, termattr_t attr, termcolor_t text, termcolor_t background)
{
    if (line < term->height)
    {
        if (column < term->width)
        {
            termcell_t *cell = &term->screen[line][column];
            cell->c = c;
            cell->attr = attr;
            cell->text = text;
            cell->background = background;
        }
    }
}

void term_clear(term_t * term)
{
    term->cursor.line = 0;
    term->cursor.column = 0;

    for(size_t i = 0; i < term->height; i++)
    {
        memset(term->screen[i], 0, sizeof(termcell_t) * term->width);
    }   
}

void term_clear_line(term_t * term, uint line, uint column)
{
    if (line < term->height && column < term->width)
    {
        memset(term->screen[line] + column, 0, sizeof(termcell_t) * (term->width - column));
    }
}

void read_ansi_attr(term_t * term, uint attr)
{
    if ( attr < 10)
    {
        term->attr = (termattr_t) attr;
    }
    else if (attr >= 30 && attr <= 37)
    {
        term->attr = (termattr_t) attr - 30;
    }
    else if (attr >= 40 && attr <= 47)
    {
        term->attr = (termattr_t) attr - 40;
    }
}

void term_print(term_t * term, const char * msg)
{
    for(size_t i = 0; msg[i]; i++)
    {
        if (msg[i] == ANSI_ESC)
        {
            const char * esc = &msg[i];
            ansitype_t type = ansi_get_type(esc);
            int value_count = ansi_value_count(esc);

            if (type != ANSI_UNKNOW)
            {
                switch (type)
                {
                    case ANSI_CLEAR:
                        term_clear(term);
                        break;
                
                    case ANSI_CLEAR_LINE:
                        term_clear_line(term, term->cursor.line, term->cursor.column);
                        break;

                    case ANSI_SET_MODE:
                        
                        for(int j = 0; j < value_count; i++)
                        {
                            read_ansi_attr(term, ansi_value(esc, j));
                        }
                        
                        break;

                    default:
                        break;
                }

                i += ansi_len(&msg[i]);
            }
        }
        else
        {
            term_cell(term, term->cursor.line, term->cursor.column, msg[i], term->attr, term->text, term->background);
            term->cursor.column++;
        }

        if (term->cursor.column >= term->width)
        {
            term->cursor.column -= term->width;
            term->cursor.line++;
        }

        if (term->cursor.line >= term->height)
        {
            term->cursor.line--;
            term_scroll(term);
        }
    } 
}