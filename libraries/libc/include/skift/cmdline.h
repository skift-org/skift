#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

struct s_cmdline;
struct s_cmdline_option;

typedef void cmdline_callback_t(
    struct s_cmdline *cmdline,
    struct s_cmdline_option *opt);

typedef enum
{
    CMDLINE_BOOLEAN,
    CMDLINE_STRING,
    CMDLINE_INTEGER,
    CMDLINE_ACTION,
    CMDLINE_SECTION,
    CMDLINE_SEPARATOR,
    CMDLINE_END
} cmdline_option_type_t;

typedef struct s_cmdline_option
{
    cmdline_option_type_t type;

    void *value;
    const char *long_name;
    const char short_name;

    cmdline_callback_t *callback;

    const char *help;
} cmdline_option_t;

typedef struct s_cmdline
{
    const char *name;
    const char *const *usages;
    const char *prologue;
    cmdline_option_t *options;
    const char *epiloge;
} cmdline_t;

#define CMDLINE_NEWLINE "\n\t"

#define CMDLINE_NO_LONG_NAME NULL
#define CMDLINE_NO_SHORT_NAME '\0'
#define CMDLINE_NO_CALLBACK NULL
#define CMDLINE_NO_VALUE NULL

#define CMDLINE_OPT_SECTION(__name)          \
    {                                        \
        .type = CMDLINE_SECTION,             \
        .value = CMDLINE_NO_VALUE,           \
        .long_name = __name,                 \
        .short_name = CMDLINE_NO_SHORT_NAME, \
        .callback = CMDLINE_NO_CALLBACK,     \
        .help = NULL,                        \
    }

#define CMDLINE_OPT_SEPARATOR              \
    {                                        \
        .type = CMDLINE_SEPARATOR,             \
        .value = CMDLINE_NO_VALUE,           \
        .long_name = NULL,                   \
        .short_name = CMDLINE_NO_SHORT_NAME, \
        .callback = CMDLINE_NO_CALLBACK,     \
        .help = NULL,                        \
    }

#define CMDLINE_OPT_BOOL(__long_name, __short_name, __value, __help, __callback) \
    {                                                                            \
        .type = CMDLINE_BOOLEAN,                                                 \
        .value = &(__value),                                                     \
        .long_name = __long_name,                                                \
        .short_name = __short_name,                                              \
        .callback = __callback,                                                  \
        .help = __help,                                                          \
    }

#define CMDLINE_OPT_STRING(__long_name, __short_name, __value, __help, __callback) \
    {                                                                              \
        .type = CMDLINE_STRING,                                                    \
        .value = &(__value),                                                       \
        .long_name = __long_name,                                                  \
        .short_name = __short_name,                                                \
        .callback = __callback,                                                    \
        .help = __help,                                                            \
    }

#define CMDLINE_OPT_INT(__long_name, __short_name, __value, __help, __callback) \
    {                                                                           \
        .type = CMDLINE_INTEGER,                                                \
        .value = &(__value),                                                    \
        .long_name = __long_name,                                               \
        .short_name = __short_name,                                             \
        .callback = __callback,                                                 \
        .help = __help,                                                         \
    }

#define CMDLINE_OPT_ACTION(__long_name, __short_name, __help, __callback) \
    {                                                                     \
        .type = CMDLINE_ACTION,                                           \
        .value = CMDLINE_NO_VALUE,                                        \
        .long_name = __long_name,                                         \
        .short_name = __short_name,                                       \
        .callback = __callback,                                           \
        .help = __help,                                                   \
    }

#define CMDLINE_OPT_HELP                            \
    {                                               \
        .type = CMDLINE_BOOLEAN,                    \
        .value = NULL,                              \
        .long_name = "help",                        \
        .short_name = 'h',                          \
        .callback = cmdline_callback_help,          \
        .help = "Show this help message and exit.", \
    }

#define CMDLINE_OPT_END      \
    {                        \
        .type = CMDLINE_END, \
    }

#define CMDLINE(__usages, __options, __prologue, __epiloge) \
    {                                                       \
        .usages = __usages,                                 \
        .prologue = __prologue,                             \
        .options = __options,                               \
        .epiloge = __epiloge                                \
    }

void cmdline_callback_help(cmdline_t *cmdline, cmdline_option_t *option);
int cmdline_parse(cmdline_t *cmdline, int argc, char **argv);