#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <skift/Environment.h>

#include <stdio.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_level(LOGGER_TRACE);

    logger_info("Loading environement variables...");

    environment() = json::parse_file("/Configs/environment.json");

    if (filesystem_exist(FRAMEBUFFER_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        logger_info("Starting settings-service...");
        process_run("settings-service", nullptr);

        int splash_pid = -1;
        process_run("splash-screen", &splash_pid);
        process_wait(splash_pid, nullptr);

        logger_info("Starting desktop environement...");
        int compositor_pid = -1;
        process_run("compositor", &compositor_pid);
        process_wait(compositor_pid, nullptr);
    }
    else
    {
        stream_format(err_stream, "No framebuffer! Starting a shell, good luck :^)\n");

        int shell_pid = -1;
        process_run("shell", &shell_pid);
        process_wait(shell_pid, nullptr);
    }

    printf("\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return PROCESS_SUCCESS;
}
