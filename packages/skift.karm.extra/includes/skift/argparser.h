#pragma once

#include <skift/generic.h>

struct s_argparser;
struct s_argparser_option;

typedef int argparser_callback_t(
    struct s_argparser *self,
    struct s_argparser_option *option,
    void *value);

typedef enum
{
    ARGPARSER_TYPE_FLAG,
    ARGPARSER_TYPE_INTEGER,
    ARGPARSER_TYPE_STRING,
    ARGPARSER_TYPE_END,
} argparser_option_type_t;

typedef struct s_argparser_option
{
    argparser_option_type_t type;
    const char short_name;
    const char *long_name;
    const char *help;

    argparser_callback_t *callback;

    void *value;
} argparser_option_t;

typedef struct s_argparser
{
    argparser_option_t *options;
    const char *description;
    const char *epilogue;
} argparser_t;

argparser_t *argparser(
    argparser_option_t *options,
    const char *description,
    const char *epilogue);

int argparser_parse(argparser_t *self, int argc, char **argv);
void argparser_delete(argparser_t *self);