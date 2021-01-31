
/* cmdline.c: skiftOS command line application utils                          */

#include <libsystem/Assert.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/NumberParser.h>

/* --- Private functions ---------------------------------------------------- */

CommandLineOption *get_long_option(CommandLine *cmdline, const char *name)
{
    for (int i = 0; cmdline->options[i].type != COMMANDLINE_END; i++)
    {
        if (strcmp(cmdline->options[i].long_name, name) == 0)
        {
            return &cmdline->options[i];
        }
    }

    return nullptr;
}

CommandLineOption *get_short_option(CommandLine *cmdline, const char name)
{
    for (int i = 0; cmdline->options[i].type != COMMANDLINE_END; i++)
    {
        if (cmdline->options[i].short_name == name)
        {
            return &cmdline->options[i];
        }
    }

    return nullptr;
}

void do_option(CommandLine *cmdline, CommandLineOption *option, int i, int argc, char **argv)
{
    if (option->value != nullptr)
    {
        switch (option->type)
        {
        case COMMANDLINE_BOOLEAN:
            *((bool *)option->value) = true;
            break;

        case COMMANDLINE_STRING:
            if (i + 1 < argc && argv[i + 1] != nullptr)
            {
                *((char **)option->value) = argv[i + 1];
                argv[i + 1] = nullptr;
            }
            break;

        case COMMANDLINE_INTEGER:
            if (i + 1 < argc && argv[i + 1] != nullptr)
            {
                *((int *)option->value) = parse_int_inline(PARSER_DECIMAL, argv[i + 1], 0);
                argv[i + 1] = nullptr;
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

size_t cmdline_option_padding(CommandLineOption *options)
{
    size_t maximal_length = 0;

    for (int i = 0; options[i].type != COMMANDLINE_END; i++)
    {
        CommandLineOption *opt = &options[i];
        size_t long_length = strlen(opt->long_name);

        if (long_length > maximal_length)
        {
            maximal_length = long_length;
        }
    }

    return maximal_length + 1;
}

void cmdline_callback_help(CommandLine *cmdline, CommandLineOption *option)
{
    __unused(option);

    if (cmdline->prologue)
    {
        printf("%s\n\n", cmdline->prologue);
    }

    if (cmdline->usages != nullptr)
    {
        printf("\e[1mUsages:\e[0m ");

        for (int i = 0; cmdline->usages[i]; i++)
        {
            printf("%s %s\n\t", cmdline->name, cmdline->usages[i]);
        }

        printf("\n");
    }

    size_t padding = cmdline_option_padding(cmdline->options);

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

            if (opt->long_name != nullptr)
            {
                printf(", --%s", opt->long_name);
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

            if (opt->long_name == nullptr)
            {
                printf("  ");
            }

            if (opt->description != nullptr)
            {
                size_t pad_length = padding - strlen(opt->long_name);

                for (size_t i = 0; i < pad_length; i++)
                {
                    printf(" ");
                }

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

        if (current_argument == nullptr)
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

            if (option == nullptr)
            {
                stream_format(err_stream, "Unknow option '%s'!\n", current_argument + 2);
                process_exit(-1);
            }

            do_option(cmdline, option, i, argc, argv);
        }
        else
        {
            for (int j = 0; current_argument[1 + j]; j++)
            {
                CommandLineOption *option = get_short_option(cmdline, current_argument[1 + j]);

                if (option == nullptr)
                {
                    stream_format(err_stream, "Unknow option '%c'!\n", current_argument[1 + j]);
                    process_exit(-1);
                }

                do_option(cmdline, option, i, argc, argv);
            }
        }

        argv[i] = nullptr;
    }

    // Packing argv
    int index = 0;

    for (int i = 0; i < argc; i++)
    {
        if (argv[i] != nullptr)
        {
            argv[index] = argv[i];
            index++;
        }
    }

    return index;
}
