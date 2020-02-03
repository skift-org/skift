/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* cmdline.c: skiftOS command line application utils                          */

#include <libsystem/assert.h>
#include <libsystem/cmdline.h>
#include <libsystem/convert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

/* --- Private functions ---------------------------------------------------- */

CommandLineOption *get_long_option(CommandLine *cmdline, const char *name)
{
    for (int i = 0; cmdline->options[i].type != COMMANDLINE_END; i++)
        if (strcmp(cmdline->options[i].long_name, name) == 0)
            return &cmdline->options[i];

    return NULL;
}

CommandLineOption *get_short_option(CommandLine *cmdline, const char name)
{
    for (int i = 0; cmdline->options[i].type != COMMANDLINE_END; i++)
        if (cmdline->options[i].short_name == name)
            return &cmdline->options[i];

    return NULL;
}

void do_option(CommandLine *cmdline, CommandLineOption *option, int i, int argc, char **argv)
{
    if (option->value != NULL)
    {
        switch (option->type)
        {
        case COMMANDLINE_BOOLEAN:
            *((bool *)option->value) = true;
            break;

        case COMMANDLINE_STRING:
            if (i + 1 < argc && argv[i + 1] != NULL)
            {
                *((char **)option->value) = argv[i + 1];
                argv[i + 1] = NULL;
            }
            break;

        case COMMANDLINE_INTEGER:
            if (i + 1 < argc && argv[i + 1] != NULL)
            {
                *((int *)option->value) = convert_string_to_uint(argv[i + 1], strnlen(argv[i + 1], 33), 10);
                argv[i + 1] = NULL;
            }
            break;

        default:
            ASSERT_NOT_REACHED();
        }
    }

    if (option->callback)
    {
        option->callback(cmdline, option);
    }
}

void cmdline_callback_help(CommandLine *cmdline, CommandLineOption *option)
{
    __unused(option);

    if (cmdline->prologue)
    {
        printf("%s\n\n", cmdline->prologue);
    }

    if (cmdline->usages != NULL)
    {
        printf("\e[1mUsages:\e[0m ");

        for (int i = 0; cmdline->usages[i]; i++)
        {
            printf("%s %s\n\t", cmdline->name, cmdline->usages[i]);
        }

        printf("\n");
    }

    printf("\e[1mOptions:\e[0m");
    for (int i = 0; cmdline->options[i].type != COMMANDLINE_END; i++)
    {
        CommandLineOption *opt = &cmdline->options[i];
        if (opt->type == COMMANDLINE_SEPARATOR)
        {
            //printf("\n");
        }
        else if (opt->type == COMMANDLINE_SECTION)
        {
            printf("\e[1m%s:\e[0m", opt->long_name);
        }
        else
        {
            if (opt->short_name != '\0')
            {
                printf(" -%c", opt->short_name);
            }
            else
            {
                printf("    ");
            }

            if (opt->long_name != NULL)
            {
                printf(", --%s ", opt->long_name);
            }
            else
            {
                printf("  ");
            }

            if (opt->type == COMMANDLINE_STRING ||
                opt->type == COMMANDLINE_INTEGER)
            {
                printf("[value] ");
            }

            if (opt->long_name == NULL)
            {
                printf("  ");
            }

            printf("\t");

            if (opt->description != NULL)
            {
                printf("%s", opt->description);
            }
        }

        printf("\n\t");
    }

    printf("\n");

    if (cmdline->epiloge)
    {
        printf(cmdline->epiloge);
        printf("\n");
    }

    process_exit(0);
}

/* --- Public functions ----------------------------------------------------- */

int cmdline_parse(CommandLine *cmdline, int argc, char **argv)
{
    cmdline->name = argv[0];

    // Parsing arguments
    for (int i = 1; i < argc; i++)
    {
        char *current_argument = argv[i];

        if (current_argument == NULL)
        {
            continue;
        }

        if (current_argument[0] != '-')
        {
            continue;
        }

        if (current_argument[1] == '-')
        {
            CommandLineOption *option = get_long_option(cmdline, current_argument + 2);

            if (option == NULL)
            {
                stream_printf(err_stream, "Unknow option '%s'!\n", current_argument + 2);
                process_exit(-1);
            }

            do_option(cmdline, option, i, argc, argv);
        }
        else
        {
            for (int j = 0; current_argument[1 + j]; j++)
            {
                CommandLineOption *option = get_short_option(cmdline, current_argument[1 + j]);

                if (option == NULL)
                {
                    stream_printf(err_stream, "Unknow option '%c'!\n", current_argument[1 + j]);
                    process_exit(-1);
                }

                do_option(cmdline, option, i, argc, argv);
            }
        }

        argv[i] = NULL;
    }

    // Packing argv
    int index = 0;

    for (int i = 0; i < argc; i++)
    {
        if (argv[i] != NULL)
        {
            argv[index] = argv[i];
            index++;
        }
    }

    return index;
}