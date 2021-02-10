#include <string.h>

#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/unicode/UnicodeString.h>
#include <libsystem/utils/BufferBuilder.h>

static void unicode_string_ensure_capacity(UnicodeString *string, size_t size)
{
    size = MAX(16, size);

    if (string->allocated >= size)
    {
        return;
    }

    if (string->buffer)
    {
        string->buffer = (Codepoint *)realloc(string->buffer, size * sizeof(Codepoint));
    }
    else
    {
        string->buffer = (Codepoint *)calloc(size, sizeof(Codepoint));
    }

    string->allocated = size;
}

UnicodeString *unicode_string_create(size_t size)
{
    UnicodeString *string = __create(UnicodeString);

    unicode_string_ensure_capacity(string, size);
    string->used = 0;

    return string;
}

void unicode_string_destroy(UnicodeString *string)
{
    free(string->buffer);
    free(string);
}

UnicodeString *unicode_string_clone(UnicodeString *original)
{
    UnicodeString *string = __create(UnicodeString);

    unicode_string_ensure_capacity(string, original->used);
    memcpy(string->buffer, original->buffer, original->used * sizeof(Codepoint));
    string->used = original->used;

    return string;
}

void unicode_string_copy(UnicodeString *source, UnicodeString *destination)
{
    unicode_string_ensure_capacity(destination, source->used);
    memcpy(destination->buffer, source->buffer, source->used * sizeof(Codepoint));
    destination->used = source->used;
}

bool unicode_string_equals(UnicodeString *left, UnicodeString *right)
{
    if (left->used != right->used)
    {
        return false;
    }

    for (size_t i = 0; i < left->used; i++)
    {
        if (left->buffer[i] != right->buffer[i])
        {
            return false;
        }
    }

    return true;
}

void unicode_string_insert(UnicodeString *string, Codepoint codepoint, size_t where)
{
    size_t needed = MAX(string->used, where) + 1;

    if (needed > string->allocated)
    {
        size_t new_allocated = MAX(string->allocated + string->allocated / 4, needed);
        unicode_string_ensure_capacity(string, new_allocated);
    }

    for (size_t i = needed - 1; i > where; i--)
    {
        string->buffer[i] = string->buffer[i - 1];
    }

    string->buffer[where] = codepoint;

    string->used = needed;
}

void unicode_string_remove(UnicodeString *string, size_t where)
{
    if (string->used > 0)
    {
        for (size_t i = where; i < string->used; i++)
        {
            string->buffer[i] = string->buffer[i + 1];
        }

        string->used--;
    }
}

size_t unicode_string_length(UnicodeString *string)
{
    return string->used;
}

char *unicode_string_as_cstring(UnicodeString *string)
{
    BufferBuilder *builder = buffer_builder_create(string->used + string->used / 4);

    for (size_t i = 0; i < string->used; i++)
    {
        uint8_t utf8[5];
        size_t size = codepoint_to_utf8(string->buffer[i], utf8);
        buffer_builder_append_str_size(builder, (const char *)utf8, size);
    }

    return buffer_builder_finalize(builder);
}

void unicode_string_clear(UnicodeString *string)
{
    string->used = 0;
}
