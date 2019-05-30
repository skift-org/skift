#include <skift/cstring.h>
#include <hjert/task.h>

#include "shell.h"

int shell_split(shell_t* this)
{
    strleadtrim(this->command_string, ' ');
    strtrailtrim(this->command_string, ' ');

    if (strlen(this->command_string) == 0)
        return 0;

    int token_index = 0;

    char **tokens = malloc(TASK_ARGV_COUNT * sizeof(char *));

    memset(tokens, 0, TASK_ARGV_COUNT * sizeof(char *));
    char *start = &this->command_string[0];

    for (size_t i = 0; i < strlen(this->command_string) + 1; i++)
    {
        char c = this->command_string[i];

        if (c == ' ' || c == '\0')
        {
            int buffer_len = &this->command_string[i] - start + 1;

            if (buffer_len > 1)
            {
                char *buffer = malloc(buffer_len);
                memcpy(buffer, start, buffer_len);
                buffer[buffer_len - 1] = '\0';

                tokens[token_index++] = buffer;
            }

            start = &this->command_string[i] + 1;
        }
    }

    tokens[token_index] = NULL;

    this->command_argv = tokens;
    this->command_argc = token_index;

    return token_index;
}

void shell_cleanup(shell_t* this)
{
    for (int i = 0; i < TASK_ARGV_COUNT; i++)
    {
        free(this->command_argv[i]);
    }

    free(this->command_argv);
}