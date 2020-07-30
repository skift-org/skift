#pragma once

#include <libsystem/Common.h>

typedef uint32_t Codepoint;

bool codepoint_is_digit(Codepoint codepoint);

bool codepoint_is_alpha(Codepoint codepoint);

int codepoint_numeric_value(Codepoint codepoint);

int utf8_to_codepoint(const uint8_t *buffer, Codepoint *codepoint);

int codepoint_to_utf8(Codepoint codepoint, uint8_t *buffer);

char codepoint_to_cp437(Codepoint codepoint);

template <typename T>
void codepoint_foreach(const uint8_t *buffer, T callback)
{
    Codepoint codepoint = 0;

    size_t size = utf8_to_codepoint(buffer, &codepoint);
    buffer += size;

    while (size && codepoint != 0)
    {
        callback(codepoint);

        size = utf8_to_codepoint(buffer, &codepoint);
        buffer += size;
    }
}
