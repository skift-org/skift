#include <libsystem/Result.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "Shell/Builtin.h"
#include "Shell/Eval.h"

int shell_eval_command(int argc, const char **argv)
{
    int command_result = 0;
    ShellBuiltinCallback builtin = shell_get_builtin(argv[0]);

    if (builtin)
    {
        command_result = builtin(argc, argv);
        goto cleanup_and_return;
    }

    char executable[PATH_LENGHT];
    snprintf(executable, PATH_LENGHT, "/bin/%s", argv[0]);

    Launchpad *launchpad = launchpad_create(argv[0], executable);

    for (int i = 0; i < argc; i++)
    {
        launchpad_argument(launchpad, argv[i]);
    }

    int process = launchpad_launch(launchpad);

    if (process < 0)
    {
        printf("%s: Command not found! \e[90m%s\e[m\n", argv[0], result_to_string(-process));

        command_result = -1;
        goto cleanup_and_return;
    }

    process_wait(process, &command_result);

cleanup_and_return:

    return command_result;
}