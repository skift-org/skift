#include <abi/Paths.h>
#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include <libtest/AssertFalse.h>
#include <libtest/AssertTrue.h>

void start_service(const char *command, const char *socket)
{
    logger_info("Starting '%s'...", command);
    int compositor_pid = -1;
    process_run(command, &compositor_pid);

    while (!filesystem_exist(socket, FILE_TYPE_SOCKET))
    {
        process_sleep(100);
    }
}

void start_desktop()
{
    int splash_screen_pid;
    process_run("splash-screen", &splash_screen_pid);

    start_service("settings-service", "/Session/settings.ipc");
    start_service("compositor", "/Session/compositor.ipc");
    process_run("panel", nullptr);

    if constexpr (__CONFIG_IS_RELEASE__)
    {
        process_run("onboarding", nullptr);
    }
    else
    {
        process_run("terminal", nullptr);
    }

    process_cancel(splash_screen_pid);

    process_sleep(-1);
}

void start_test()
{
    int test_pid;
    process_run("tests", &test_pid);
    int test_result;
    process_wait(test_pid, &test_result);
    hj_system_shutdown();
}

void start_headless()
{
    int shell_pid = -1;
    process_run("shell", &shell_pid);
    process_wait(shell_pid, nullptr);
}

int main(int, const char *[])
{
    logger_level(LOGGER_TRACE);

    logger_info("Loading environement variables...");
    environment() = json::parse_file("/Configs/environment.json");

    if constexpr (__CONFIG_IS_TEST__)
    {
        logger_info("Running in testing mode...");
        start_test();
    }
    else if (filesystem_exist(FRAMEBUFFER_DEVICE_PATH, FILE_TYPE_DEVICE))
    {
        logger_info("Running in graphical mode...");
        start_desktop();
    }
    else
    {
        logger_warn("No framebuffer! Starting a shell, good luck :^)");
        start_headless();
    }

    stream_format(err_stream, "\n\n\t\e[1;34mGoodbye!\e[m - n°1\n\n");

    return PROCESS_SUCCESS;
}
