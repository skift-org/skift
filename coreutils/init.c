#include <abi/Paths.h>

#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/Logger.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    if (filesystem_exist(FRAMEBUFFER_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        int compositor_pid = -1;
        process_run("compositor", &compositor_pid);
        process_wait(compositor_pid, NULL);
    }
    else if (filesystem_exist(TEXTMODE_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        int terminal_pid = -1;
        process_run("terminal", &terminal_pid);
        process_wait(terminal_pid, NULL);
    }
    else
    {
        int shell_pid = -1;
        process_run("shell", &shell_pid);
        process_wait(shell_pid, NULL);
    }

    printf("\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return 0;
}
