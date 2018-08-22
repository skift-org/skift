#pragma once
#include "types.h"
#include "sync/lock.h"

typedef enum
{
    TERMATTR_NONE = 0,
    TERMATTR_BOLD = 1,
    TERMATTR_DIM = 2,
    TERMATTR_ITALIC = 3,
    TERMATTR_UNDERSCORE = 4,
    TERMATTR_BLINK = 5,
    TERMATTR_REVERSE = 7,
    TERMATTR_CONCEALED = 8
} termattr_t;

typedef enum
{
    TERMCOLOR_BLACK = 0,
    TERMCOLOR_RED = 1,
    TERMCOLOR_GREEN = 2,
    TERMCOLOR_YELLOW = 3,
    TERMCOLOR_BLUE = 4,
    TERMCOLOR_MAGENTA = 5,
    TERMCOLOR_CYAN = 6,
    TERMCOLOR_WHITE = 7,
} termcolor_t;

typedef struct
{
    char c;

    termattr_t attr;
    termcolor_t text;
    termcolor_t background;
} termcell_t;

typedef struct 
{
    uint line;
    uint column;
} termcusor_t;

typedef struct
{
    termcusor_t cursor;
    termcell_t **screen;

    uint width;
    uint height;

    termattr_t attr;
    termcolor_t text;
    termcolor_t background;

    lock_t lock;
} term_t;

term_t *term_alloc(uint width, uint height);
void term_free(term_t *term);

void term_scroll(term_t *term);
void term_cell(term_t *term, uint line, uint column, char c, termattr_t attr, termcolor_t text, termcolor_t background);
void term_cursor(term_t *term, uint line, uint column);
void term_print(term_t * term, const char * msg);