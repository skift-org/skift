/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>
#include <skift/iostream.h>
#include <skift/logger.h>

#include "shell.h"

int main(int argc, char **argv)
{
    logger_setlevel(LOG_ALL);

    // HACK: Disable line buffering
    out_stream->write_buffer = 0;
    out_stream->write_buffer = NULL;

    (void)argc;
    (void)argv;

    shell_t *this = &(shell_t){
        .do_continue = true,
        .builtins = shell_get_builtins(),
        .command_exit_value = 0,
        .command_string = malloc(MAX_COMMAND_LENGHT),
    };

    while (this->do_continue)
    {
        shell_prompt(this);

        if (shell_readline(this) == 0)
        {
            printf("Empty command!\n");

            this->command_exit_value = -1;
            continue;
        }

        if (shell_split(this) == 0)
        {
            printf("Empty command!\n");

            this->command_exit_value = -1;
            continue;
        }

        if (!shell_eval(this)) 
        {
            printf("Command not found!\n");
        }

        shell_cleanup(this);
    }

    return this->command_exit_value;
}
