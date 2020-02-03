#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

struct scanf_info;

typedef void (*scanf_append_t)(struct printf_info *info, char c);
typedef int (*scanf_getc_t)(struct scanf_info *info);

typedef enum
{
    SFSTATE_ESC,
    SFSTATE_PARSE,
    SFSTATE_FORMAT_LENGHT,
    SFSTATE_FINALIZE
} scanf_state_t;

typedef struct scanf_info
{
    char c;

    scanf_state_t state;
    scanf_getc_t getc;
    scanf_append_t append;

    const char *format;
    int format_offset;

    // Scanner
    uint lenght;

    void *p;
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