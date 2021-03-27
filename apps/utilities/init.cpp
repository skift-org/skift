#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libsystem/Logger.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/process/Process.h>

void start_service(const char *command, const char *socket)
{
    logger_info("Starting '%s'...", command);
    int compositor_pid = -1;
    process_run(command, &compositor_pid, TASK_WAITABLE);

    while (!filesystem_exist(socket, FILE_TYPE_SOCKET))
    {
        process_sleep(100);
    }
}

void start_desktop()
{
    int splash_screen_pid;
    process_run("splash-screen", &splash_screen_pid, 0);

    start_service("settings-service", "/Session/settings.ipc");
    start_service("compositor", "/Session/compositor.ipc");
    process_run("panel", nullptr, 0);

    if constexpr (__CONFIG_IS_RELEASE__)
    {
        process_run("onboarding", nullptr, 0);
    }
    else
    {
        process_run("terminal", nullptr, 0);
    }

    process_cancel(splash_screen_pid);

    process_sleep(-1);
}

void start_test()
{
    int test_pid;
    process_run("tests", &test_pid, TASK_WAITABLE);
    int test_result;
    process_wait(test_pid, &test_result);
    hj_system_shutdown();
}

void start_headless()
{
    int shell_pid = -1;
    process_run("shell", &shell_pid, TASK_WAITABLE);
    process_wait(shell_pid, nullptr);
}

int main(int, const char *[])
{
    logger_level(LOGGER_TRACE);

    logger_info("Loading environement variables...");
    environment() = Json::parse_file("/Configs/environment.json");

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
