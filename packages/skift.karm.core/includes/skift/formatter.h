#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

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

typedef struct 
{
    char c;
    printf_state_t state;

    char* output;
    uint  output_offset;
    uint  output_size;

    const char* format;
    uint format_offset;

    char padding;
    printf_align_t align;
    uint lenght;
} printf_info_t;

typedef int (*formatter_t)(printf_info_t* info, void* v);

#define PADDING(buffer, a)                                 \
do                                                             \
{                                                              \
    if (info->align == a && strlen(buffer) < info->lenght) \
    {                                                          \
        for(uint i = 0; i < info->lenght - strlen(buffer); i++) \
        {                                                      \
            APPEND(info->padding);                             \
        }                                                      \
    }                                                          \
} while(0)

#define APPEND(c)                                     \
do                                                    \
{                                                     \
    if (info->output_offset >= info->output_size - 1) \
        return strlen(info->output);                  \
    info->output[info->output_offset++] = c;          \
    info->output[info->output_offset] = '\0';         \
} while(0)

#define PEEK()                                        \
do                                                    \
{                                                     \
    info->c = info->format[info->format_offset++];    \
    if (info->c == '\0') return strlen(info->output); \
} while(0)

void sk_formatter_init();
bool sk_formatter_register(char c, formatter_t formatter);
int sk_formatter_format(printf_info_t* info, char sel, va_list* va);
//TODO: for fprintf
// int sk_formatter_format_stream(printf_info_t* info, char sel, void* v);

