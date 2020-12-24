#pragma once

#include <libsystem/Common.h>

struct NumberParser
{
    int base;
};

#define PARSER_BINARY ((NumberParser){2})
#define PARSER_OCTAL ((NumberParser){8})
#define PARSER_DECIMAL ((NumberParser){10})
#define PARSER_HEXADECIMAL ((NumberParser){16})

unsigned int parse_uint_inline(NumberParser parser, const char *str, unsigned int default_value);

bool parse_uint(NumberParser parser, const char *str, size_t size, unsigned int *result);

int parse_int_inline(NumberParser parser, const char *str, int default_value);

bool parse_int(NumberParser parser, const char *str, size_t size, int *result);
