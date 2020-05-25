#pragma once

#include <libsystem/Common.h>

typedef struct
{
    size_t allocated;
    size_t used;
    size_t cursor;
    size_t old_cursor;

    char *buffer;
} ReadLine;

ReadLine *readline_create(void);

void readline_destroy(ReadLine *readline);

Result readline_readline(ReadLine *readline, char **line);
