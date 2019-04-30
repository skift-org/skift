#pragma once

#include <skift/runtime.h>

struct s_printf_info;

typedef void (*printf_append_t)(struct s_printf_info *info, char c);

typedef enum
{
    PFSTATE_ESC,
    PFSTATE_PARSE,
    PFSTATE_FORMAT_LENGHT,
    PFSTATE_FINALIZE
} printf_state_t;

typedef enum
{
    PFALIGN_LEFT,
    PFALIGN_RIGHT
} printf_align_t;

typedef struct s_printf_info
{
    char c;
    printf_state_t state;
    printf_append_t append;

    const char *format;
    int format_offset;

    // Formating
    char padding;
    printf_align_t align;
    uint lenght;

    void *p;
    int n;
    int max_n;
} printf_info_t;

typedef int (*printf_formatter_impl_t)(printf_info_t *info, va_list *va);

typedef struct
{
    char c;
    printf_formatter_impl_t impl;
} printf_formatter_t;

#define PRINTF_PEEK()                                  \
    {                                                  \
        info->c = info->format[info->format_offset++]; \
        if (info->c == '\0')                           \
            return info->n;                            \
    }

#define PRINTF_APPEND(__c)                               \
    {                                                    \
        if (info->max_n != -1 && info->n >= info->max_n) \
            return info->n;                              \
                                                         \
        info->append(info, __c);                         \
        info->n++;                                       \
    }

#define PRINTF_PADDING(__buffer, __a)                                  \
    {                                                                  \
        if (info->align == __a && strlen(__buffer) < info->lenght)     \
        {                                                              \
            for (uint i = 0; i < info->lenght - strlen(__buffer); i++) \
            {                                                          \
                PRINTF_APPEND(info->padding);                          \
            }                                                          \
        }                                                              \
    }

int __printf(printf_info_t *info, va_list va);