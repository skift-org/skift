#pragma once

#include <skift/utils.h>
#include <skift/types.h>

typedef struct 
{
    uint size;
    uint allocated_size;

    wchar_t * buffer;
} string_t;

string_t * string(const wchar_t * cstring);
void string_delete(string_t * str);


