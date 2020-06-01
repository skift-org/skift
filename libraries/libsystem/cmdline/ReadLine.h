#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/unicode/UTF8Decoder.h>
#include <libsystem/unicode/UnicodeString.h>
#include <libsystem/utils/SourceReader.h>

typedef struct
{
    bool should_continue;

    size_t cursor;
    size_t old_cursor;

    Stream *stream;
    UTF8Decoder *decoder;
    UnicodeString *string;
    SourceReader *reader;
} ReadLine;

ReadLine *readline_create(Stream *stream);

void readline_destroy(ReadLine *readline);

Result readline_readline(ReadLine *readline, char **line);
