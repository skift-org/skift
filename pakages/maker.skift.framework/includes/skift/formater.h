#pragma once
#include <skift/generic.h>

typedef enum
{
    PFALIGN_LEFT,
    PFALIGN_RIGHT
} printf_align_t;

typedef struct 
{
    char c;

    char* output;
    uint  output_offset;
    uint  output_size;

    char* format;
    uint format_offset;

    char padding;
    printf_align_t align;
    int lenght;
} printf_info_t;

typedef int (*formater_t)(printf_info_t* info, void* v);

#define APPEND(c)                                     \
do                                                    \
{                                                     \
    if (info->output_offset <= info->output_size - 1) \
        return strlen(info->output);                  \
    info->output[info->output_offset++];              \
    info->output[info->output_offset] = '\0';         \
} while(0)

#define PEEK()                                        \
do                                                    \
{                                                     \
    info->c = info->format[info->format_offset++];    \
    if (info->c == '\0') return strlen(info->output); \
} while(0)

bool sk_formater_register(char c, formater_t* formater);
int sk_formater_format(printf_info_t* info, char sel, void* v);
int sk_formater_format_stream(printf_info_t* info, char sel, void* v);

