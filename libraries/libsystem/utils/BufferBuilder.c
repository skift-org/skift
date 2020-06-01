#include <libmath/MinMax.h>
#include <libsystem/Assert.h>
#include <libsystem/utils/BufferBuilder.h>

BufferBuilder *buffer_builder_create(size_t preallocated)
{
    preallocated = MAX(preallocated, 16);

    BufferBuilder *buffer = __create(BufferBuilder);

    buffer->buffer = (char *)calloc(preallocated, sizeof(char));
    buffer->size = preallocated;
    buffer->used = 0;

    return buffer;
}

void buffer_builder_destroy(BufferBuilder *buffer)
{
    if (buffer->buffer)
    {
        free(buffer->buffer);
    }

    free(buffer);
}

char *buffer_builder_finalize(BufferBuilder *buffer)
{
    char *result = buffer->buffer;
    buffer->buffer = NULL;

    buffer_builder_destroy(buffer);

    return result;
}

void buffer_builder_append_str(BufferBuilder *buffer, const char *str)
{
    if (str)
    {
        for (size_t i = 0; str[i]; i++)
        {
            buffer_builder_append_chr(buffer, str[i]);
        }
    }
    else
    {
        buffer_builder_append_str(buffer, "<null>");
    }
}

void buffer_builder_append_str_size(BufferBuilder *buffer, const char *str, size_t size)
{
    if (str)
    {
        for (size_t i = 0; i < size; i++)
        {
            buffer_builder_append_chr(buffer, str[i]);
        }
    }
    else
    {
        buffer_builder_append_str(buffer, "<null>");
    }
}

void buffer_builder_append_chr(BufferBuilder *buffer, char chr)
{
    if (buffer->used + 1 == buffer->size)
    {
        buffer->size *= 1.25;
        buffer->buffer = (char *)realloc(buffer->buffer, buffer->size);
    }

    buffer->buffer[buffer->used] = chr;
    buffer->buffer[buffer->used + 1] = '\0';
    buffer->used++;
}

void buffer_builder_rewind(BufferBuilder *buffer, size_t how_many)
{
    assert(buffer->used >= how_many);

    buffer->used -= how_many;
    buffer->buffer[buffer->used] = '\0';
}
