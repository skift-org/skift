#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    int splash_pid = -1;
    process_run("splash-screen", &splash_pid);
    process_wait(splash_pid, nullptr);

    // Start a "DEBUG" shell
    // process_run("shell", NULL);

    if (filesystem_exist(FRAMEBUFFER_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        int compositor_pid = -1;
        process_run("compositor", &compositor_pid);
        process_wait(compositor_pid, nullptr);
    }
    else if (filesystem_exist(TEXTMODE_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        int terminal_pid = -1;
        process_run("terminal", &terminal_pid);
        process_wait(terminal_pid, nullptr);
    }

    printf("\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return PROCESS_SUCCESS;
}
