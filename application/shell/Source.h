#pragma once

#include <libsystem/io/Stream.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/unicode/UTF8Decoder.h>

struct Source;

typedef Codepoint (*SourceNextCodepointCallback)(struct Source *source);
typedef void (*SourceDestroyCallback)(struct Source *source);

typedef struct Source
{
    Codepoint current_cp;
    Codepoint peek_cp;
    bool is_eof;

    UTF8Decoder *decoder;

    SourceNextCodepointCallback next_codepoint;
    SourceDestroyCallback destroy;
} Source;

#define SHELL_SOURCE(__subclass) ((Source *)(__subclass))

typedef struct
{
    Source source;

    Stream *stream;
} StreamSource;

typedef struct
{
    Source source;

    const char *buffer;
    size_t size;
    size_t offset;
} StringSource;

Source *source_create_from_stream(Stream *stream);

Source *source_create_from_string(const char *buffer, size_t size);

void source_destroy(Source *source);