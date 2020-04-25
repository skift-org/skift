#pragma once

#include <libsystem/runtime.h>

typedef struct
{
    size_t used;
    size_t size;
    char *buffer;
} BufferBuilder;

BufferBuilder *buffer_builder_create(size_t preallocated);

void buffer_builder_destroy(BufferBuilder *buffer);

char *buffer_builder_finalize(BufferBuilder *buffer);

void buffer_builder_append_str(BufferBuilder *buffer, const char *str);

void buffer_builder_append_str_size(BufferBuilder *buffer, const char *str, size_t size);

void buffer_builder_append_chr(BufferBuilder *buffer, char chr);

void buffer_builder_rewind(BufferBuilder *buffer, size_t how_many);
