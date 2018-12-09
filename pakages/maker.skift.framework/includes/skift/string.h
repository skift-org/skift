#pragma once
#include <skift/generic.h>

#define sk_strlen(str)        sk_string_lenght(str)
#define sk_strapdc(dest, c)   sk_string_append_char(dest, c)
#define sk_strapds(dest, str) sk_string_append_string(dest, str)

#define sk_stoui(str, base)        sk_string_to_uint(str, base)
#define sk_uitos(value, str, base) sk_uint_to_string(value, str, base)

/* --- String basic operations ---------------------------------------------- */

uint sk_string_lenght(const char *str);
char* sk_string_append_char(char *str, char c);
char* sk_string_append_string(char *dest, const char *str);

uint sk_string_to_uint(const char *str, uchar base);
void sk_uint_to_string(uint value, char *str, uchar base);