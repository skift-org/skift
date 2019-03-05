#include <string.h>
#include <stdlib.h>
#include <skift/cmdline.h>

/* --- Private functions ---------------------------------------------------- */

cmdline_option_t *get_long_option(cmdline_t *cmdline, const char *name)
{
    for (int i = 0; cmdline->options[i].type != CMDLINE_END; i++)
        if (strcmp(cmdline->options[i].long_name, name) == 0)
            return &cmdline->options[i];

    return NULL;
}

cmdline_option_t *get_short_option(cmdline_t *cmdline, const char name)
{
    for (int i = 0; cmdline->options[i].type != CMDLINE_END; i++)
        if (cmdline->options[i].short_name == name)
            return &cmdline->options[i];

    return NULL;
}

void do_option(cmdline_t *cmdline, cmdline_option_t *option, int i, int argc, char **argv)
{
    switch (option->type)
    {
    case CMDLINE_BOOLEAN:
    {
        if (option->value != NULL)
            *((bool *)option->value) = true;

        break;
    }
    case CMDLINE_STRING:
    {
        if (option->value != NULL)
            if (i + 1 < argc && argv[i + 1] != NULL)
            {
                *((char **)option->value) = argv[i + 1];
                argv[i + 1] = NULL;
            }

        break;
    }
    case CMDLINE_INTEGER:
    {
        if (option->value != NULL)
            if (i + 1 < argc && argv[i + 1] != NULL)
            {
                *((int *)option->value) = stoi(argv[i + 1], 10);
                argv[i + 1] = NULL;
            }

        break;
    }

    default:
        break;
    }

    if (option->callback != NULL)
    {
        option->callback(cmdline, option);
    }
}

void cmdline_callback_help(cmdline_t *cmdline, cmdline_option_t *option)
{
    UNUSED(option);

    if (cmdline->usages != NULL)
    {
        printf("Usages: ");

        for(int i = 0; cmdline->usages[i]; i++)
        {
            printf("%s\n\t", cmdline->usages[i]);
        }
        
        printf("\n");
    }

    if (cmdline->prologue)
    {
        printf("Prologue:\n\t");
        printf(cmdline->prologue);
        printf("\n\n");
    }

    printf("Options:\n\t");
    printf("\n\n");

    if (cmdline->epiloge)
    {
        printf("Epilogue:\n\t");
        printf(cmdline->epiloge);
        printf("\n\n");
    }
}

/* --- Public functions ----------------------------------------------------- */

int cmdline_parse(cmdline_t *cmdline, int argc, char **argv)
{
    // Parsing arguments
    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        if (arg[0] == '-')
        {
            cmdline_option_t *opt = NULL;

            if (arg[1] == '-')
            {
                opt = get_long_option(cmdline, arg + 2);

                if (opt != NULL)
                {
                    do_option(cmdline, opt, i, argc, argv);
                }
            }
            else
            {
                for (int j = 0; arg[1 + j]; j++)
                {
                    opt = get_short_option(cmdline, arg[1 + j]);

                    if (opt != NULL)
                    {
                        do_option(cmdline, opt, i, argc, argv);
                    }
                }
            }

            argv[i] = NULL;
        }
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