#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Filesystem.h>
#include <libsystem/Result.h>
#include <libsystem/io/Pipe.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "shell/Shell.h"

Result shell_exec(ShellCommand *command, Stream *stdin, Stream *stdout, int *pid)
{
    char executable[PATH_LENGTH];
    if (command->command[0] == '/' ||
        command->command[0] == '.')
    {
        snprintf(executable, PATH_LENGTH, "%s", command->command);
    }
    else
    {
        snprintf(executable, PATH_LENGTH, "/bin/%s", command->command);
    }

    Launchpad *launchpad = launchpad_create(command->command, executable);

    list_foreach(char, arg, command->arguments)
    {
        launchpad_argument(launchpad, arg);
    }

    launchpad_handle(launchpad, HANDLE(stdin), 0);
    launchpad_handle(launchpad, HANDLE(stdout), 1);

    Result result = launchpad_launch(launchpad, pid);

    if (result != SUCCESS)
    {
        printf("%s: Command not found! \e[90m%s\e[m\n", command->command, result_to_string(result));
    }

    return result;
}

int shell_eval(ShellNode *node, Stream *stdin, Stream *stdout)
{
    switch (node->type)
    {
    case SHELL_NODE_COMMAND:
    {
        ShellCommand *command = (ShellCommand *)node;

        ShellBuiltinCallback builtin = shell_get_builtin(command->command);

        if (builtin)
        {
            // list_count(command->arguments) + 1 for argv[0] which is the command name.
            char **argv = (char **)calloc(list_count(command->arguments) + 1, sizeof(argv));
            argv[0] = command->command;
            int argc = 1;

            list_foreach(char, arg, command->arguments)
            {
                argv[argc] = arg;
                argc++;
            }

            int result = builtin(argc, (const char **)argv);
            free(argv);

            return result;
        }

        int pid;
        Result result = shell_exec(command, stdin, stdout, &pid);

        if (result == SUCCESS)
        {
            int command_result = -1;
            process_wait(pid, &command_result);
            return command_result;
        }
    }
    break;

    case SHELL_NODE_PIPELINE:
    {
        ShellPipeline *pipeline = (ShellPipeline *)node;

        List *pipes = list_create();

        for (int i = 0; i < list_count(pipeline->commands) - 1; i++)
        {
            list_pushback(pipes, pipe_create());
        }

        int *processes = (int *)calloc(list_count(pipeline->commands), sizeof(int));

        for (int i = 0; i < list_count(pipeline->commands); i++)
        {
            ShellCommand *command = NULL;
            list_peekat(pipeline->commands, i, (void **)&command);
            assert(command);

            Stream *command_stdin = stdin;
            Stream *command_stdout = stdout;

            if (i > 0)
            {
                Pipe *input_pipe;
                assert(list_peekat(pipes, i - 1, (void **)&input_pipe));
                command_stdin = input_pipe->out;
            }

            if (i < list_count(pipeline->commands) - 1)
            {
                Pipe *output_pipe;
                assert(list_peekat(pipes, i, (void **)&output_pipe));
                command_stdout = output_pipe->in;
            }

            shell_exec(command, command_stdin, command_stdout, &processes[i]);
        }

        list_destroy_with_callback(pipes, (ListDestroyElementCallback)pipe_destroy);

        for (int i = 0; i < list_count(pipeline->commands); i++)
        {
            int exit_value;
            process_wait(processes[i], &exit_value);
        }

        free(processes);

        return 0;
    }
    break;

    case SHELL_NODE_REDIRECT:
    {
        ShellRedirect *redirect = (ShellRedirect *)node;

        __cleanup(stream_cleanup) Stream *stream = stream_open(redirect->destination, OPEN_WRITE | OPEN_CREATE);

        if (handle_has_error(stream))
        {
            handle_printf_error(stream, "Failled to open '%s'", redirect->destination);
            return -1;
        }

        return shell_eval(redirect->command, stdin, stream);
    }
    break;

    default:
        break;
    }

    return -1;
}
