#include <stdlib.h>
#include <string.h>

/* --- 7.22 - General utilities --------------------------------------------- */

/* --- 7.22.1 - Numeric conversion functions -------------------------------- */

// double atof(char const *nptr);

// int atoi(char const *nptr);

// long atol(char const *nptr);

// long long atoll(char const *nptr);

// long int strtol(char const *STDC_RESTRICT nptr,
//                 char **STDC_RESTRICT endptr,
//                 int base);

// long long int strtoll(char const *STDC_RESTRICT nptr,
//                       char **STDC_RESTRICT endptr,
//                       int base);

// unsigned long int strtoul(char const *STDC_RESTRICT nptr,
//                           char **STDC_RESTRICT endptr,
//                           int base);

// unsigned long long int strtoull(char *const STDC_RESTRICT nptr,
//                                 char **STDC_RESTRICT endptr,
//                                 int base);

// double strtod(char const *STDC_RESTRICT nptr,
//               char **STDC_RESTRICT endptr);

// float strtof(char const *STDC_RESTRICT nptr,
//              char **STDC_RESTRICT endptr);

// long double strtold(char const *STDC_RESTRICT nptr, char **STDC_RESTRICT endptr);

// int strfromd(char *STDC_RESTRICT s,
//              size_t n,
//              char const *STDC_RESTRICT format,
//              double fp);

// int strfromf(char *STDC_RESTRICT s,
//              size_t n,
//              char const *STDC_RESTRICT format,
//              floatf p);

// int strfroml(char *STDC_RESTRICTs,
//              size_t n,
//              char const *STDC_RESTRICT format,
//              long double fp);

/* --- 7.22.2 - Pseudo-random sequence generation functions ----------------- */

// int rand();

// void srand(unsigned int seed);

/* --- 7.22.3 - Memory management functions --------------------------------- */

// void *aligned_alloc(size_t alignment, size_t size);

// void *calloc(size_t nmemb, size_t size) ;

// void free(void *ptr) ;;

// void *malloc(size_t size);

// void *realloc(void *ptr, size_t size);

/* --- 7.22.4 - Communication with the environment -------------------------- */

// _Noreturn void abort(void);

int atexit(void (*)(void)) {
    // FIXME: Not implemented
    return 0;
}

// int at_quick_exit(void (*func)(void));

// _Noreturn void exit(int status);

// _Noreturn void _Exit(int status);

// char *getenv(char const *name);

// _Noreturn void quick_exit(int status);

// int system(char const *string);

/* --- 7.22.5 - Searching and sorting utilities ----------------------------- */

// void *bsearch(void const *key,
//               void const *base,
//               size_t nmemb,
//               size_t size,
//               int (*compar)(void const *, void const *));

// void qsort(void *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *))
// In qsort.c

/* --- 7.22.6 - Integer arithmetic functions -------------------------------- */

int abs(int j) {
    if (j < 0) {
        return -j;
    }

    return j;
}

long labs(long j) {
    if (j < 0) {
        return -j;
    }

    return j;
}

long long llabs(long long j) {
    if (j < 0) {
        return -j;
    }

    return j;
}

div_t div(int number, int denom) {
    div_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

ldiv_t ldiv(long number, long denom) {
    ldiv_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

lldiv_t lldiv(long long number, long long denom) {
    lldiv_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

/* --- 7.22.7 - Multibyte/wide character conversion functions --------------- */

// int mblen(char const *s, size_t n);

// int mbtowc(wchar_t *STDC_RESTRICT pwc, char const *STDC_RESTRICT s, size_t n);

// int wctomb(char *s, wchar_t wc);

// size_t mbstowcs(wchar_t *STDC_RESTRICT pwcs, char const *STDC_RESTRICT s, size_t n);

// size_t mcstombs(char *STDC_RESTRICT s, wchar_t const *STDC_RESTRICT pwcs, size_t n);
