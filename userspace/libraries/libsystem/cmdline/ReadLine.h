#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/unicode/UTF8Decoder.h>
#include <libsystem/unicode/UnicodeString.h>
#include <libutils/Scanner.h>

struct ReadLine
{
    bool should_continue;

    size_t cursor;
    size_t old_cursor;

    size_t history_current;

    Stream *stream;
    UTF8Decoder *decoder;
    UnicodeString *string;
    StreamScanner *scan;
};

ReadLine *readline_create(Stream *stream);

void readline_destroy(ReadLine *readline);

Result readline_readline(ReadLine *readline, char **line);
