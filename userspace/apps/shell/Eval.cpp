#include <skift/Environment.h>

#include <libio/File.h>
#include <libio/Format.h>
#include <libio/Pipe.h>
#include <libio/Streams.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <libutils/Assert.h>

#include "shell/Shell.h"

Optional<String> find_command_path(String command)
{
    if (command[0] == '/' || command[0] == '.')
    {
        IO::File file{command};
        return file.exist() ? command : Optional<String>{};
    }
    else
    {
        auto application_path = IO::format("/Applications/{}/{}", command, command);

        IO::File application_file{application_path};
        if (application_file.exist())
        {
            return application_path;
        }

        auto &path = environment().get("POSIX").get("PATH");

        for (size_t i = 0; i < path.length(); i++)
        {
            auto utility_path = IO::format("{}/{}", path.get(i).as_string(), command);

            IO::File utility_file{utility_path};
            if (utility_file.exist())
            {
                return utility_path;
            }
        }
    }

    return {};
}

Result shell_exec(
    ShellCommand *command,
    RefPtr<IO::Handle> instream,
    RefPtr<IO::Handle> outstream,
    int *pid)
{
    auto executable = find_command_path(command->command);

    if (!executable.present())
    {
        *pid = -1;
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    Launchpad *launchpad = launchpad_create(command->command, executable.unwrap().cstring());
    launchpad_flags(launchpad, TASK_WAITABLE);

    list_foreach(char, arg, command->arguments)
    {
        launchpad_argument(launchpad, arg);
    }

    launchpad_handle(launchpad, *instream, 0);
    launchpad_handle(launchpad, *outstream, 1);

    return launchpad_launch(launchpad, pid);
}

int shell_eval(ShellNode *node, RefPtr<IO::Handle> instream, RefPtr<IO::Handle> outstream)
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
            char **argv = (char **)calloc(command->arguments->count() + 1, sizeof(argv));
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
        Result result = shell_exec(command, instream, outstream, &pid);
        auto path = IO::Path::parse(command->command, IO::Path::PARENT_SHORTHAND);

        if (result == SUCCESS)
        {
            int command_result = -1;
            process_wait(pid, &command_result);
            return command_result;
        }
        else if (filesystem_exist(path.string().cstring(), FILE_TYPE_DIRECTORY))
        {
            process_set_directory(path.string().cstring());

            return PROCESS_SUCCESS;
        }
        else
        {
            IO::errln("{}: Command not found! \e[90m{}\e[m", command->command, result_to_string(result));
            return PROCESS_FAILURE;
        }
    }
    break;

    case SHELL_NODE_PIPELINE:
    {
        ShellPipeline *pipeline = (ShellPipeline *)node;

        Vector<IO::Pipe> pipes;

        for (int i = 0; i < pipeline->commands->count() - 1; i++)
        {
            pipes.push_back(IO::Pipe::create().unwrap());
        }

        Vector<int> processes;

        for (int i = 0; i < pipeline->commands->count(); i++)
        {
            ShellCommand *command = nullptr;
            list_peekat(pipeline->commands, i, (void **)&command);
            Assert::is_true(command);

            RefPtr<IO::Handle> command_stdin = instream;
            RefPtr<IO::Handle> command_stdout = outstream;

            if (i > 0)
            {
                command_stdin = pipes[i - 1].reader;
            }

            if (i < pipeline->commands->count() - 1)
            {
                command_stdout = pipes[i].writer;
            }

            shell_exec(command, command_stdin, command_stdout, &processes.emplace_back(-1));
        }

        pipes.clear();

        int exit_value;

        for (int i = 0; i < pipeline->commands->count(); i++)
        {
            process_wait(processes[i], &exit_value);
        }

        return exit_value;
    }
    break;

    case SHELL_NODE_REDIRECT:
    {
        ShellRedirect *redirect = (ShellRedirect *)node;

        IO::File file{redirect->destination, OPEN_WRITE | OPEN_CREATE};

        if (file.exist())
        {
            IO::errln("Failed to open '{}'", redirect->destination);
            return PROCESS_FAILURE;
        }

        return shell_eval(redirect->command, instream, file.handle());
    }
    break;

    default:
        break;
    }

    return -1;
}
