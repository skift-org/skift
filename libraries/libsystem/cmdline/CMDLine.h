#pragma once

#include <libsystem/Common.h>

struct CommandLine;
struct CommandLineOption;

typedef void CommandLineCallback(struct CommandLine *cmdline, struct CommandLineOption *opt);

enum CommandLineOptionType
{
    COMMANDLINE_BOOLEAN,
    COMMANDLINE_STRING,
    COMMANDLINE_INTEGER,
    COMMANDLINE_ACTION,
    COMMANDLINE_SECTION,
    COMMANDLINE_SEPARATOR,
    COMMANDLINE_END
};

struct CommandLineOption
{
    CommandLineOptionType type;

    void *value;
    const char short_name;
    const char *long_name;
    const char *description;

    CommandLineCallback *callback;
};

struct CommandLine
{
    const char *name;
    const char *const *usages;
    const char *prologue;
    CommandLineOption *options;
    const char *epiloge;
};

#define COMMANDLINE_NEWLINE "\n\t"

#define COMMANDLINE_NO_LONG_NAME nullptr
#define COMMANDLINE_NO_SHORT_NAME '\0'
#define COMMANDLINE_NO_CALLBACK nullptr
#define COMMANDLINE_NO_VALUE nullptr

#define COMMANDLINE_OPT_SECTION(__name)          \
    {                                            \
        .type = COMMANDLINE_SECTION,             \
        .value = COMMANDLINE_NO_VALUE,           \
        .short_name = COMMANDLINE_NO_SHORT_NAME, \
        .long_name = __name,                     \
        .description = nullptr,                  \
        .callback = COMMANDLINE_NO_CALLBACK,     \
    }

#define COMMANDLINE_OPT_SEPARATOR                \
    {                                            \
        .type = COMMANDLINE_SEPARATOR,           \
        .value = COMMANDLINE_NO_VALUE,           \
        .short_name = COMMANDLINE_NO_SHORT_NAME, \
        .long_name = nullptr,                    \
        .description = nullptr,                  \
        .callback = COMMANDLINE_NO_CALLBACK,     \
    }

#define COMMANDLINE_OPT_BOOL(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                       \
        .type = COMMANDLINE_BOOLEAN,                                                        \
        .value = &(__value),                                                                \
        .short_name = __short_name,                                                         \
        .long_name = __long_name,                                                           \
        .description = __description,                                                       \
        .callback = __callback,                                                             \
    }

#define COMMANDLINE_OPT_STRING(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                         \
        .type = COMMANDLINE_STRING,                                                           \
        .value = &(__value),                                                                  \
        .short_name = __short_name,                                                           \
        .long_name = __long_name,                                                             \
        .description = __description,                                                         \
        .callback = __callback,                                                               \
    }

#define COMMANDLINE_OPT_INT(__long_name, __short_name, __value, __description, __callback) \
    {                                                                                      \
        .type = COMMANDLINE_INTEGER,                                                       \
        .value = &(__value),                                                               \
        .short_name = __short_name,                                                        \
        .long_name = __long_name,                                                          \
        .description = __description,                                                      \
        .callback = __callback,                                                            \
    }

#define COMMANDLINE_OPT_ACTION(__long_name, __short_name, __description, __callback) \
    {                                                                                \
        .type = COMMANDLINE_ACTION,                                                  \
        .value = COMMANDLINE_NO_VALUE,                                               \
        .short_name = __short_name,                                                  \
        .long_name = __long_name,                                                    \
        .description = __description,                                                \
        .callback = __callback,                                                      \
    }

#define COMMANDLINE_OPT_HELP                               \
    {                                                      \
        .type = COMMANDLINE_BOOLEAN,                       \
        .value = nullptr,                                  \
        .short_name = 'h',                                 \
        .long_name = "help",                               \
        .description = "Show this help message and exit.", \
        .callback = cmdline_callback_help,                 \
    }

#define COMMANDLINE_OPT_END      \
    {                            \
        .type = COMMANDLINE_END, \
                                 \
        .value = nullptr,        \
        .short_name = 'x',       \
        .long_name = "",         \
        .description = "",       \
                                 \
        .callback = nullptr,     \
    }

#define CMDLINE(__usages, __options, __prologue, __epiloge) \
    {                                                       \
        .name = "<null>",                                   \
        .usages = __usages,                                 \
        .prologue = __prologue,                             \
        .options = __options,                               \
        .epiloge = __epiloge                                \
    }

void cmdline_callback_help(CommandLine *cmdline, CommandLineOption *option);
int cmdline_parse(CommandLine *cmdline, int argc, char **argv);
