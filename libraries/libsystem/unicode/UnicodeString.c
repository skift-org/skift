
#include <libmath/MinMax.h>
#include <libsystem/Logger.h>
#include <libsystem/unicode/UnicodeString.h>
#include <libsystem/utils/BufferBuilder.h>

UnicodeString *unicode_string_create(size_t size)
{
    UnicodeString *string = __create(UnicodeString);

    size = MAX(16, size);

    string->buffer = (Codepoint *)calloc(size, sizeof(Codepoint));
    string->allocated = size;
    string->used = 0;

    return string;
}

void unicode_string_destroy(UnicodeString *string)
{
    free(string->buffer);
    free(string);
}

void unicode_string_insert(UnicodeString *string, Codepoint codepoint, size_t where)
{
    size_t needed = MAX(string->used, where) + 1;

    if (needed > string->allocated)
    {
        size_t new_allocated = MAX(string->allocated * 1.25, needed);
        string->buffer = (Codepoint *)realloc(string->buffer, new_allocated * sizeof(Codepoint));
        string->allocated = new_allocated;
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
    BufferBuilder *builder = buffer_builder_create(string->used * 1.1);

    for (size_t i = 0; i < string->used; i++)
    {
        uint8_t utf8[5];
        size_t size = codepoint_to_utf8(string->buffer[i], utf8);
        buffer_builder_append_str_size(builder, (const char *)utf8, size);
    }

    char *str = buffer_builder_finalize(builder);

    return str;
}

void unicode_string_clear(UnicodeString *string)
{
    string->used = 0;
}
