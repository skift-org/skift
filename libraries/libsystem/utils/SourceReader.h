#pragma once

#include <libsystem/io/Stream.h>
#include <libsystem/utils/RingBuffer.h>

typedef struct
{
    Stream *stream;
    /*    or    */
    const char *string;

    size_t size;
    size_t offset;
    RingBuffer *peek;
} SourceReader;

SourceReader *source_create_from_stream(Stream *stream);

SourceReader *source_create_from_string(const char *string, size_t size);

void source_destroy(SourceReader *source);

bool source_ended(SourceReader *source);

bool source_do_continue(SourceReader *source);

void source_foreward(SourceReader *source);

#define SOURCE_READER_MAX_PEEK 16
char source_peek(SourceReader *source, size_t peek);

char source_current(SourceReader *source);

bool source_current_is(SourceReader *source, const char *what);

bool source_current_is_word(SourceReader *source, const char *word);

void source_eat(SourceReader *source, const char *what);

bool source_skip(SourceReader *source, char chr);

bool source_skip_word(SourceReader *source, const char *word);

// Read **JSON** escape sequence like \n \t \r or \uXXXX
const char *source_read_escape_sequence(SourceReader *source);
