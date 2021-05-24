#pragma once

#include <libutils/Prelude.h>

struct NumberFormater
{
    int base;
    int after_point;
    bool padded_with_zero;
    bool capitalized;
};

#define FORMAT_BINARY ((NumberFormater){2, 4, true, false})
#define FORMAT_OCTAL ((NumberFormater){8, 4, true, false})
#define FORMAT_DECIMAL ((NumberFormater){10, 4, false, false})
#define FORMAT_HEXADECIMAL ((NumberFormater){16, 4, true, false})

size_t format_uint(NumberFormater formater, unsigned long value, char *str, size_t size);

size_t format_int(NumberFormater formater, long value, char *str, size_t size);

#ifndef __KERNEL__

size_t format_double(NumberFormater formater, double value, char *str, size_t size);

#endif
