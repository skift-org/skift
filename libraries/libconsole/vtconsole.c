/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/ctype.h>
#include <libmath/math.h>
#include <libconsole/vtconsole.h>

/* --- Constructor/Destructor ----------------------------------------------- */

vtconsole_t *vtconsole(int width, int height, vtc_paint_handler_t on_paint, vtc_cursor_handler_t on_move)
{
    vtconsole_t *vtc = malloc(sizeof(vtconsole_t));

    vtc->width = width;
    vtc->height = height;

    vtc->ansiparser = (vtansi_parser_t){VTSTATE_ESC, {{0, 0}}, 0};
    vtc->attr = VTC_DEFAULT_ATTR;

    vtc->buffer = malloc(width * height * sizeof(vtcell_t));

    vtc->cursor = (vtcursor_t){0, 0};

    vtc->on_paint = on_paint;
    vtc->on_move = on_move;

    vtconsole_clear(vtc, 0, 0, width, height - 1);

    return vtc;
}

void vtconsole_delete(vtconsole_t *vtc)
{
    free(vtc->buffer);
    free(vtc);
}

/* --- Internal methodes ---------------------------------------------------- */

void vtconsole_clear(vtconsole_t *vtc, int fromx, int fromy, int tox, int toy)
{
    for (int i = fromx + fromy * vtc->width; i < tox + toy * vtc->width; i++)
    {
        vtconsole_update_cell(vtc, i % vtc->width, i / vtc->width, ' ', VTC_DEFAULT_ATTR);
    }
}

void vtconsole_scroll(vtconsole_t *vtc)
{
    // Scroll the screen.
    for (int i = 0; i < ((vtc->width * vtc->height) - vtc->width); i++)
    {
        vtconsole_update_cell_with_cell(vtc, i % vtc->width, i / vtc->width, vtc->buffer[i + vtc->width]);
    }

    // Clear the last line.
    for (int i = ((vtc->width * vtc->height) - vtc->width); i < vtc->width * vtc->height; i++)
    {
        vtconsole_update_cell(vtc, i % vtc->width, i / vtc->width, ' ', VTC_DEFAULT_ATTR);
    }

    // Move the cursor up a line.
    if (vtc->cursor.y > 0)
    {
        vtc->cursor.y--;

        if (vtc->on_move)
        {
            vtc->on_move(vtc, &vtc->cursor);
        }
    }
}

// Append a new line
void vtconsole_newline(vtconsole_t *vtc)
{
    vtc->cursor.x = 0;
    vtc->cursor.y++;

    if (vtc->cursor.y >= vtc->height)
        vtconsole_scroll(vtc);

    if (vtc->on_move)
    {
        vtc->on_move(vtc, &vtc->cursor);
    }
}

// Append character to the console buffer.
void vtconsole_append(vtconsole_t *vtc, char c)
{
    if (c == '\n')
    {
        vtconsole_newline(vtc);
    }
    else if (c == '\r')
    {
        vtc->cursor.x = 0;

        if (vtc->on_move)
        {
            vtc->on_move(vtc, &vtc->cursor);
        }
    }
    else if (c == '\t')
    {
        int n = 8 - (vtc->cursor.x % 8);

        for (int i = 0; i < n; i++)
        {
            vtconsole_append(vtc, ' ');
        }
    }
    else if (c == '\b')
    {
        if (vtc->cursor.x > 0)
        {
            vtc->cursor.x--;
        }
        else
        {
            vtc->cursor.y--;
            vtc->cursor.x = vtc->width - 1;
        }

        if (vtc->on_move)
        {
            vtc->on_move(vtc, &vtc->cursor);
        }
    }
    else
    {
        if (vtc->cursor.x >= vtc->width)
            vtconsole_newline(vtc);


        vtconsole_update_cell(vtc, vtc->cursor.x, vtc->cursor.y, c, vtc->attr);

        vtc->cursor.x++;

        if (vtc->on_move)
        {
            vtc->on_move(vtc, &vtc->cursor);
        }
    }
}

void vtconsole_update_cell(vtconsole_t *vtc, int x, int y, char c, vtattr_t attr)
{
    vtcell_t *new_cell = &(vtcell_t){
        .c = c,
        .attr = attr,
        .is_dirty = true,
    };

    vtcell_t *old_cell = vtconsole_cell(vtc, x, y);

    if (old_cell->c != new_cell->c ||
        old_cell->attr.bg != new_cell->attr.bg ||
        old_cell->attr.fg != new_cell->attr.fg ||
        old_cell->attr.bright != new_cell->attr.bright)
    {
        *old_cell = *new_cell; 

        if (vtc->on_paint)
        {
            vtc->on_paint(vtc, old_cell, vtc->cursor.x, vtc->cursor.y);
        }
    }
}

void vtconsole_update_cell_with_cell(vtconsole_t *vtc, int x, int y, vtcell_t cell)
{
    vtconsole_update_cell(vtc, x, y, cell.c, cell.attr);
}

vtcell_t *vtconsole_cell(vtconsole_t *vtc, int x, int y)
{
    return &vtc->buffer[x + y * vtc->width];
}

// Moves the cursor to row n, column m. The values are 1-based,
void vtconsole_csi_cup(vtconsole_t *vtc, vtansi_arg_t *stack, int count)
{
    if (count == 1 && stack[0].empty)
    {
        vtc->cursor.x = 0;
        vtc->cursor.y = 0;
    }
    else if (count == 2)
    {
        if (stack[0].empty)
        {
            vtc->cursor.y = 0;
        }
        else
        {
            vtc->cursor.y = min(stack[0].value - 1, vtc->height - 1);
        }

        if (stack[1].empty)
        {
            vtc->cursor.y = 0;
        }
        else
        {
            vtc->cursor.x = min(stack[1].value - 1, vtc->width - 1);
        }
    }

    if (vtc->on_move)
    {
        vtc->on_move(vtc, &vtc->cursor);
    }
}

// Clears part of the screen.
void vtconsole_csi_ed(vtconsole_t *vtc, vtansi_arg_t *stack, int count)
{
    UNUSED(count);

    vtcursor_t cursor = vtc->cursor;

    if (stack[0].empty)
    {
        vtconsole_clear(vtc, cursor.x, cursor.y, vtc->width, vtc->height - 1);
    }
    else
    {
        int attr = stack[0].value;

        if (attr == 0)
            vtconsole_clear(vtc, cursor.x, cursor.y, vtc->width, vtc->height - 1);
        else if (attr == 1)
            vtconsole_clear(vtc, 0, 0, cursor.x, cursor.y);
        else if (attr == 2)
            vtconsole_clear(vtc, 0, 0, vtc->width, vtc->height - 1);
    }
}

// Erases part of the line.
void vtconsole_csi_el(vtconsole_t *vtc, vtansi_arg_t *stack, int count)
{
    UNUSED(count);

    vtcursor_t cursor = vtc->cursor;

    if (stack[0].empty)
    {
        vtconsole_clear(vtc, cursor.x, cursor.y, vtc->width, cursor.y);
    }
    else
    {
        int attr = stack[0].value;

        if (attr == 0)
            vtconsole_clear(vtc, cursor.x, cursor.y, vtc->width, cursor.y);
        else if (attr == 1)
            vtconsole_clear(vtc, 0, cursor.y, cursor.x, cursor.y);
        else if (attr == 2)
            vtconsole_clear(vtc, 0, cursor.y, vtc->width, cursor.y);
    }
}

// Sets the appearance of the following characters
void vtconsole_csi_sgr(vtconsole_t *vtc, vtansi_arg_t *stack, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (stack[i].empty || stack[i].value == 0)
        {
            vtc->attr = VTC_DEFAULT_ATTR;
        }
        else
        {
            int attr = stack[i].value;

            if (attr == 1) // Increased intensity
            {
                vtc->attr.bright = true;
            }
            else if (attr >= 30 && attr <= 37) // Set foreground color
            {
                vtc->attr.fg = attr - 30;
            }
            else if (attr >= 40 && attr <= 47) // Set background color
            {
                vtc->attr.bg = attr - 40;
            }
        }
    }
}

// Process ANSI escape sequences and append character to the console buffer.
void vtconsole_process(vtconsole_t *vtc, char c)
{
    vtansi_parser_t *parser = &vtc->ansiparser;

    switch (parser->state)
    {
    case VTSTATE_ESC:
        if (c == '\033')
        {
            parser->state = VTSTATE_BRACKET;

            parser->index = 0;

            parser->stack[parser->index].value = 0;
            parser->stack[parser->index].empty = true;
        }
        else
        {
            parser->state = VTSTATE_ESC;
            vtconsole_append(vtc, c);
        }
        break;

    case VTSTATE_BRACKET:
        if (c == '[')
        {
            parser->state = VTSTATE_ATTR;
        }
        else
        {
            parser->state = VTSTATE_ESC;
            vtconsole_append(vtc, c);
        }
        break;
    case VTSTATE_ATTR:
        if (isdigit(c))
        {
            parser->stack[parser->index].value *= 10;
            parser->stack[parser->index].value += (c - '0');
            parser->stack[parser->index].empty = false;
        }
        else
        {
            if ((parser->index) < VTC_ANSI_PARSER_STACK_SIZE)
            {
                parser->index++;
            }

            parser->stack[parser->index].value = 0;
            parser->stack[parser->index].empty = true;

            parser->state = VTSTATE_ENDVAL;
        }
        break;
    default:
        break;
    }

    if (parser->state == VTSTATE_ENDVAL)
    {
        if (c == ';')
        {
            parser->state = VTSTATE_ATTR;
        }
        else
        {
            switch (c)
            {
            case 'H':
                vtconsole_csi_cup(vtc, parser->stack, parser->index);
                break;
            case 'J':
                vtconsole_csi_ed(vtc, parser->stack, parser->index);
                break;
            case 'K':
                vtconsole_csi_el(vtc, parser->stack, parser->index);
                break;
            case 'm':
                vtconsole_csi_sgr(vtc, parser->stack, parser->index);
                break;
            }

            parser->state = VTSTATE_ESC;
        }
    }
}

/* --- Methodes ------------------------------------------------------------- */

void vtconsole_putchar(vtconsole_t *vtc, char c)
{
    vtconsole_process(vtc, c);
}

void vtconsole_write(vtconsole_t *vtc, const char *buffer, uint size)
{
    for (uint i = 0; i < size; i++)
    {
        vtconsole_process(vtc, buffer[i]);
    }
}