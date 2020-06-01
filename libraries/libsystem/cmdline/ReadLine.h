#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/unicode/UTF8Decoder.h>
#include <libsystem/unicode/UnicodeString.h>

typedef struct
{
    bool should_continue;

    size_t cursor;
    size_t old_cursor;

    UTF8Decoder *decoder;
    UnicodeString *string;
} ReadLine;

ReadLine *readline_create(void);

void readline_destroy(ReadLine *readline);

Result readline_readline(ReadLine *readline, char **line);
