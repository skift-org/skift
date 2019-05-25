#pragma once

#include <skift/runtime.h>

struct scanf_info;

typedef char (*scanf_get_t)(struct scanf_info *info);

typedef enum
{
    SCANF_STATE_ESC,
    SCANF_STATE_PARSE,
    SCANF_STATE_FORMAT_LENGHT,
    SCANF_STATE_FINALIZE
} scanf_state_t;

typedef struct scanf_info
{
    char c;

    scanf_state_t state;
    scanf_get_t get;

    const char *format;
    int format_offset;

    // Scanner
    uint lenght;

    void* p;
    int n;
    int max_n;
} scanf_info_t;

#define SCANF_PEEK()                                   \
    {                                                  \
        info->c = info->format[info->format_offset++]; \
        if (info->c == '\0')                           \
            return info->n;                            \
    }

typedef int (*scanf_scanner_impl_t)(scanf_info_t *info, va_list *va);

typedef struct
{
    char c;
    scanf_scanner_impl_t impl;
} scanf_scanner_t;

int __scanf(scanf_info_t *info, va_list va);