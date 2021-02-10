#pragma once

#include <libutils/unicode/Codepoint.h>

struct UnicodeString
{
    Codepoint *buffer;
    size_t used;
    size_t allocated;
};

UnicodeString *unicode_string_create(size_t size);

void unicode_string_destroy(UnicodeString *string);

UnicodeString *unicode_string_clone(UnicodeString *string);

void unicode_string_copy(UnicodeString *source, UnicodeString *destination);

bool unicode_string_equals(UnicodeString *left, UnicodeString *right);

void unicode_string_insert(UnicodeString *string, Codepoint codepoint, size_t where);

void unicode_string_remove(UnicodeString *string, size_t where);

size_t unicode_string_length(UnicodeString *string);

char *unicode_string_as_cstring(UnicodeString *string);

void unicode_string_clear(UnicodeString *string);
