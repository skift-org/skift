#include <libsystem/CString.h>
#include <libsystem/Filesystem.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "shell/Shell.h"

int shell_eval(ShellNode *node, Stream *stdin, Stream *stdout)
{
    switch (node->type)
    {
    case SHELL_NODE_COMMAND:
    {
        ShellNodeCommand *command = (ShellNodeCommand *)node;

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

        char executable[PATH_LENGTH];
        snprintf(executable, PATH_LENGTH, "/bin/%s", command->command);

        Launchpad *launchpad = launchpad_create(command->command, executable);

        list_foreach(char, arg, command->arguments)
        {
            launchpad_argument(launchpad, arg);
        }

        launchpad_handle(launchpad, HANDLE(stdin), 0);
        launchpad_handle(launchpad, HANDLE(stdin), 1);

        int pid = -1;
        Result result = launchpad_launch(launchpad, &pid);

        if (result != SUCCESS)
        {
            printf("%s: Command not found! \e[90m%s\e[m\n", command->command, result_to_string(result));

            return -1;
        }
        else
        {
            int command_result = -1;
            process_wait(pid, &command_result);
            return command_result;
        }
    }
    break;

    case SHELL_NODE_PIPE:
    {
        ShellNodeOperator *pipe = (ShellNodeOperator *)node;

        Stream *reader = NULL;
        Stream *writter = NULL;

        stream_create_pipe(&reader, &writter);

        shell_eval(pipe->left, stdin, writter);

        shell_eval(pipe->right, reader, stdout);

        stream_close(reader);
        stream_close(writter);

        return 0;
    }
    break;

    default:
        break;
    }

    return -1;
}
