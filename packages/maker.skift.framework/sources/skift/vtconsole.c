#include <skift/vtconsole.h>

/* --- Constructor/Destructor ----------------------------------------------- */

vtconsole_t *vtconsole(uint width, uint height, vtc_paint_handler_t on_paint)
{
    vtconsole_t *vtc = MALLOC(vtconsole_t);

    vtc->width = width;
    vtc->height = height;

    vtc->ansiparser = (vtansi_parser_t){VTSTATE_ESC, {0}, 0};
    vtc->attr = VTC_DEFAULT_ATTR;

    vtc->buffer = malloc(width * height * sizeof(vtcell_t));

    for (uint i = 0; i < width * height; i++)
    {
        vtcell_t *cell = &vtc->buffer[i];
        cell->c = ' ';
        cell->attr = VTC_DEFAULT_ATTR;
    }

    vtc->cursor = (vtcursor_t){0, 0};

    vtc->on_paint = on_paint;
}

void vtconsole_delete(vtconsole_t *vtc)
{
    free(vtc->buffer);
    free(vtc);
}

/* --- Internal methodes ---------------------------------------------------- */

// Set the cursor position
void vtconsole_set_cursor(vtconsole_t *vtc, uint x, uint y)
{
    vtc->cursor.x = min(x, vtc->width);
    vtc->cursor.y = min(y, vtc->height);

    if (vtc->on_move)
    {
        vtc->on_move(vtc, &vtc->cursor);
    }
}

void vtconsole_move_cursor(vtconsole_t *vtc, int offx, int offy)
{
    vtconsole_set_cursor(vtc, vtc->cursor.x + offx, vtc->cursor.y + offy);
}

// Clear the console buffer.
void vtconsole_clear(vtconsole_t *vtc, uint fromx, uint fromy, uint tox, uint toy)
{
    for (uint i = fromx + fromy * vtc->width; i < tox + toy * vtc->width; i++)
    {
        vtcell_t *cell = &vtc->buffer[i];

        cell->attr = VTC_DEFAULT_ATTR;
        cell->c = ' ';

        if (vtc->on_paint)
        {
            vtc->on_paint(vtc, cell, i % vtc->width, i / vtc->width);
        }
    }
}

// Scroll the console
void vtconsole_scroll(vtconsole_t *vtc)
{
    for (int i = 0; i < (vtc->width * vtc->height) - vtc->width; i++)
    {
        vtc->buffer[i] = vtc->buffer[i + vtc->width];

        if (vtc->on_paint)
        {
            vtc->on_paint(vtc, &vtc->buffer[i], i % vtc->width, i / vtc->width);
        }
    }

    vtconsole_move_cursor(vtc, 0, -1);
}

// Append a new line
void vtconsole_newline(vtconsole_t *vtc)
{
    vtconsole_set_cursor(vtc, 0, vtc->cursor.y);
    vtconsole_move_cursor(vtc, 0, +1);

    if (vtc->cursor.y >= vtc->width)
    {
        vtconsole_scroll(vtc);
    }
}

// Append character to the console buffer.
void vtconsole_append(vtconsole_t *vtc, char c)
{
    if (c == '\n')
    {
        vtconsole_newline(vtc);
    }
    else if (c == '\t')
    {
        // TODO: tabs stops
        for (uint i = 0; i < 8; i++)
        {
            console_append(' ');
        }
    }
    else if (c == '\b')
    {
        if (vtc->cursor.x != 0)
        {
            vtc->cursor.x--;

            if (vtc->on_move)
            {
                vtc->on_move(vtc, &vtc->cursor);
            }
        }
    }
    else
    {
        vtcell_t *cell = &vtc->buffer[vtc->cursor.x + vtc->cursor.y * vtc->width];
        cell->c = c;
        cell->attr = vtc->attr;

        if (vtc->on_paint)
        {
            vtc->on_paint(vtc, cell, vtc->cursor.x, vtc->cursor.y);
        }

        vtc->cursor.x++;

        if (vtc->on_move)
        {
            vtc->on_move(vtc, &vtc->cursor);
        }

        if (vtc->cursor.x == vtc->width)
            vtconsole_newline(vtc);
    }
}

// Moves the cursor to row n, column m. The values are 1-based,
void vtconsole_csi_cup(vtconsole_t *vtc, uint *stack, uint count)
{
    if (count == 0)
    {
    }
}

// Clears part of the screen.
void vtconsole_csi_ed(vtconsole_t *vtc, uint *stack, uint count)
{
}

// Erases part of the line.
void vtconsole_csi_el(vtconsole_t *vtc, uint *stack, uint count)
{
}

// Sets the appearance of the following characters
void vtconsole_csi_sgr(vtconsole_t *vtc, uint *stack, uint count)
{
    for (int i = 0; i < count; i++)
    {
        uint attr = stack[i];

        if (attr == 0) // Reset
        {
            vtc->attr = VTC_DEFAULT_ATTR;
        }
        else if (attr == 1) // Increased intensity
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
            parser->stack[0] = 0;
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
            console_append(c);
        }
        break;
    case VTSTATE_ATTR:
        if (isdigit(c))
        {
            parser->stack[parser->index] *= 10;
            parser->stack[parser->index] += (c - '0');
        }
        else
        {
            if ((parser->index) < VTC_ANSI_PARSER_STACK_SIZE)
            {
                parser->index++;
            }

            parser->stack[parser->index] = 0;
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

void vtconsole_write(vtconsole_t *vtc, const char *buffer)
{
    for (int i = 0; buffer[i]; i++)
    {
        vtconsole_putchar(vtc, buffer[i]);
    }
}