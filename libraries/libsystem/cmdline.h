#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

struct CommandLine;
struct CommandLineOption;

typedef void CommandLineCallback(struct CommandLine *cmdline, struct CommandLineOption *opt);

typedef enum
{
    COMMANDLINE_BOOLEAN,
    COMMANDLINE_STRING,
    COMMANDLINE_INTEGER,
    COMMANDLINE_ACTION,
    COMMANDLINE_SECTION,
    COMMANDLINE_SEPARATOR,
    COMMANDLINE_END
} CommandLineOptionType;

typedef struct CommandLineOption
{
    CommandLineOptionType type;

    const char short_name;
    const char *long_name;
    const char *description;

    void *value;
    CommandLineCallback *callback;
} CommandLineOption;

typedef struct CommandLine
{
    const char *name;
    const char *const *usages;
    const char *prologue;
    CommandLineOption *options;
    const char *epiloge;
} CommandLine;

#define COMMANDLINE_NEWLINE "\n\t"

#define COMMANDLINE_NO_LONG_NAME NULL
#define COMMANDLINE_NO_SHORT_NAME '\0'
#define COMMANDLINE_NO_CALLBACK NULL
#define COMMANDLINE_NO_VALUE NULL

#define COMMANDLINE_OPT_SECTION(__name)          \
    {                                            \
        .type = COMMANDLINE_SECTION,             \
        .value = COMMANDLINE_NO_VALUE,           \
        .long_name = __name,                     \
        .short_name = COMMANDLINE_NO_SHORT_NAME, \
        .callback = COMMANDLINE_NO_CALLBACK,     \
        .description = NULL,                     \
    }

#define COMMANDLINE_OPT_SEPARATOR                \
    {                                            \
        .type = COMMANDLINE_SEPARATOR,           \
        .value = COMMANDLINE_NO_VALUE,           \
        .long_name = NULL,                       \
        .short_name = COMMANDLINE_NO_SHORT_NAME, \
        .callback = COMMANDLINE_NO_CALLBACK,     \
        .description = NULL,                     \
    }

#define COMMANDLINE_OPT_BOOL(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                       \
        .type = COMMANDLINE_BOOLEAN,                                                        \
        .value = &(__value),                                                                \
        .long_name = __long_name,                                                           \
        .short_name = __short_name,                                                         \
        .callback = __callback,                                                             \
        .description = __description,                                                       \
    }

#define COMMANDLINE_OPT_STRING(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                         \
        .type = COMMANDLINE_STRING,                                                           \
        .value = &(__value),                                                                  \
        .long_name = __long_name,                                                             \
        .short_name = __short_name,                                                           \
        .callback = __callback,                                                               \
        .description = __description,                                                         \
    }

#define COMMANDLINE_OPT_INT(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                      \
        .type = COMMANDLINE_INTEGER,                                                       \
        .value = &(__value),                                                               \
        .long_name = __long_name,                                                          \
        .short_name = __short_name,                                                        \
        .callback = __callback,                                                            \
        .description = __description,                                                      \
    }

#define COMMANDLINE_OPT_ACTION(__long_name, __short_name, __description, __callback) \
    {                                                                                \
        .type = COMMANDLINE_ACTION,                                                  \
        .value = COMMANDLINE_NO_VALUE,                                               \
        .long_name = __long_name,                                                    \
        .short_name = __short_name,                                                  \
        .callback = __callback,                                                      \
        .description = __description,                                                \
    }

#define COMMANDLINE_OPT_HELP                               \
    {                                                      \
        .type = COMMANDLINE_BOOLEAN,                       \
        .value = NULL,                                     \
        .long_name = "help",                               \
        .short_name = 'h',                                 \
        .callback = cmdline_callback_help,                 \
        .description = "Show this help message and exit.", \
    }

#define COMMANDLINE_OPT_END      \
    {                            \
        .type = COMMANDLINE_END, \
    }

#define CMDLINE(__usages, __options, __prologue, __epiloge) \
    {                                                       \
        .usages = __usages,                                 \
        .prologue = __prologue,                             \
        .options = __options,                               \
        .epiloge = __epiloge                                \
    }

void cmdline_callback_help(CommandLine *cmdline, CommandLineOption *option);
int cmdline_parse(CommandLine *cmdline, int argc, char **argv);
