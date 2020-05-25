
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/readline/ReadLine.h>

#include "shell/Builtin.h"
#include "shell/Eval.h"

#define PROMPT u8"\e[;1;94m µ \e[m"

void shell_prompt(int last_command_exit_value)
{
    printf("\n\e[m ");

    if (last_command_exit_value != 0)
    {
        printf("(\e[;1;31m%d\e[m) ", last_command_exit_value);
    }

    char buffer[PATH_LENGTH];
    process_get_cwd(buffer, PATH_LENGTH);
    printf("%s", buffer);

    printf(PROMPT);
}

int shell_split(char *command, char **argv)
{
    int argc = 0;

    strleadtrim(command, ' ');
    strtrailtrim(command, ' ');

    if (strlen(command) == 0)
    {
        return 0;
    }

    char *start = &command[0];

    for (size_t i = 0; i < strlen(command) + 1; i++)
    {
        char c = command[i];

        if (c == ' ' || c == '\0')
        {
            int buffer_len = &command[i] - start + 1;

            if (buffer_len > 1)
            {
                char *buffer = (char *)malloc(buffer_len);
                memcpy(buffer, start, buffer_len);
                buffer[buffer_len - 1] = '\0';

                argv[argc++] = buffer;
            }

            start = &command[i] + 1;
        }
    }

    argv[argc] = NULL;

    return argc;
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    stream_set_write_buffer_mode(out_stream, STREAM_BUFFERED_NONE);

    ReadLine *readline = readline_create();

    int command_exit_value = 0;

    while (true)
    {
        shell_prompt(command_exit_value);
        char *command = NULL;
        if (readline_readline(readline, &command) != SUCCESS)
        {
            if (command)
                free(command);
            return -1;
        }

        char *command_argv[PROCESS_ARG_COUNT] = {};
        int command_argc = shell_split(command, command_argv);

        if (command_argc == 0)
        {
            printf("Empty command!\n");

            command_exit_value = -1;
            continue;
        }

        command_exit_value = shell_eval_command(command_argc, (const char **)command_argv);

        for (int i = 0; i < command_argc; i++)
        {
            free(command_argv[i]);
        }

        free(command);
    }
}
