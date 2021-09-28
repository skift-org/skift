#include <limits.h>
#include <locale.h>

static struct lconv _locale_defaults = {
    .decimal_point = ".",
    .thousands_sep = "",
    .grouping = "",
    .mon_decimal_point = "",
    .mon_thousands_sep = "",
    .mon_grouping = "",
    .positive_sign = "",
    .negative_sign = "",
    .currency_symbol = "",
    .frac_digits = CHAR_MAX,
    .p_cs_precedes = CHAR_MAX,
    .n_cs_precedes = CHAR_MAX,
    .p_sep_by_space = 0,
    .n_sep_by_space = 0,
    .p_sign_posn = CHAR_MAX,
    .n_sign_posn = CHAR_MAX,
    .int_curr_symbol = "",
    .int_frac_digits = 0,
    .int_p_cs_precedes = CHAR_MAX,
    .int_n_cs_precedes = CHAR_MAX,
    .int_p_sep_by_space = CHAR_MAX,
    .int_n_sep_by_space = CHAR_MAX,
    .int_p_sign_posn = CHAR_MAX,
    .int_n_sign_posn = CHAR_MAX,
};

struct lconv *localeconv(void)
{
    return &_locale_defaults;
}
