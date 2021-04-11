#pragma once

#include <__libc__.h>

__BEGIN_HEADER

#define LC_ALL 1
#define LC_COLLATE 2
#define LC_CTYPE 3
#define LC_MONETARY 4
#define LC_NUMERIC 5
#define LC_TIME 6
#define LC_MESSAGES 7

struct lconv
{
    const char *decimal_point;
    const char *thousands_sep;
    const char *grouping;
    const char *mon_decimal_point;
    const char *mon_thousands_sep;
    const char *mon_grouping;
    const char *positive_sign;
    const char *negative_sign;
    const char *currency_symbol;
    char frac_digits;
    char p_cs_precedes;
    char n_cs_precedes;
    char p_sep_by_space;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    const char *int_curr_symbol;
    char int_frac_digits;
    char int_p_cs_precedes;
    char int_n_cs_precedes;
    char int_p_sep_by_space;
    char int_n_sep_by_space;
    char int_p_sign_posn;
    char int_n_sign_posn;
};

struct lconv *localeconv(void);

char *setlocale(int category, const char *locale);

__END_HEADER
