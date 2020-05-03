#include <libsystem/Result.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "shell/Builtin.h"
#include "shell/Eval.h"

int shell_eval_command(int argc, const char **argv)
{
    ShellBuiltinCallback builtin = shell_get_builtin(argv[0]);

    if (builtin)
    {
        return builtin(argc, argv);
    }
    else
    {
        char executable[PATH_LENGTH];
        snprintf(executable, PATH_LENGTH, "/bin/%s", argv[0]);

        Launchpad *launchpad = launchpad_create(argv[0], executable);

        for (int i = 0; i < argc; i++)
        {
            launchpad_argument(launchpad, argv[i]);
        }

        int pid = -1;
        Result result = launchpad_launch(launchpad, &pid);

        if (result != SUCCESS)
        {
            printf("%s: Command not found! \e[90m%s\e[m\n", argv[0], result_to_string(result));

            return -1;
        }
        else
        {
            int command_result = -1;
            process_wait(pid, &command_result);
            return command_result;
        }
    }
}