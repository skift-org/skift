#include <skift/__plugs__.h>
#include <skift/iostream.h>
#include "shell.h"

#define PROMPT "\033[0;1;34m $ \033[0;1m"

void shell_prompt(shell_t* this)
{
    // Reset the terminal
    printf("\n\033[0m ");

    // Last command exit value
    if (this->command_exit_value != 0)
    {
        printf("\e[0;1;31m%d\e[0m ", this->command_exit_value);
    }

    // Current directory
    char buffer[1024];
    __plug_process_get_workdir(buffer, 1024);
    printf("%s", buffer);

    // The prompt
    printf(PROMPT);
}