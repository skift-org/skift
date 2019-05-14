#include <skift/iostream.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/cstring.h>

#include "shell.h"

bool shell_eval(shell_t* this)
{
    // Try to exect a builtin 
    shell_builtin_handler_t builtin = shell_get_builtin(this->command_argv[0]);

    if (builtin != NULL)
    {
        this->command_exit_value = builtin(this, this->command_argc, (const char**)this->command_argv);
        return true;
    }

    // Or exec a real command
    iostream_t *s = iostream_open(this->command_argv[0], IOSTREAM_READ);

    int process = 0;

    if (s != NULL)
    {
        iostream_close(s);
        process = process_exec(this->command_argv[0], (const char**)this->command_argv);
    }
    else
    {
        char pathbuffer[144];
        sprintf(pathbuffer, 144, "/bin/%s", this->command_argv[0]);

        s = iostream_open(pathbuffer, IOSTREAM_READ);

        if (s != NULL)
        {
            iostream_close(s);
            process = process_exec(pathbuffer, (const char**)this->command_argv);
        }
    }

    if (process > 0)
    {
        thread_wait_process(process, &this->command_exit_value);
        return true;
    }

    return false;
}