/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/readline/ReadLine.h>

#include "Shell/Builtin.h"
#include "Shell/Eval.h"

#define PROMPT u8"\e[;1;94m µ \e[m"

void shell_prompt(int last_command_exit_value)
{
    printf("\n\e[m ");

    if (last_command_exit_value != 0)
    {
        printf("(\e[;1;31m%d\e[m) ", last_command_exit_value);
    }

    char buffer[PATH_LENGHT];
    process_get_cwd(buffer, PATH_LENGHT);
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
                char *buffer = malloc(buffer_len);
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
        char *command = readline_readline(readline);

        char *command_argv[PROCESS_ARG_COUNT] = {0};
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