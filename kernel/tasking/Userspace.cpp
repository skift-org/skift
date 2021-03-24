#include <libsystem/Logger.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "kernel/graphics/EarlyConsole.h"
#include "kernel/tasking/Userspace.h"

#define DEFAULT_ENVIRONMENT "{}"

void userspace_initialize()
{
    logger_info("Starting the userspace...");

    Launchpad *init_lauchpad = launchpad_create("init", "/System/Utilities/init");
    launchpad_flags(init_lauchpad, TASK_WAITABLE | TASK_USER);

    Stream *serial_device = stream_open("/Devices/serial", OPEN_WRITE | OPEN_READ);

    launchpad_handle(init_lauchpad, HANDLE(serial_device), 0);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 1);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 2);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 3);

    launchpad_environment(init_lauchpad, DEFAULT_ENVIRONMENT);

    early_console_disable(); // We disable the early console to prevent artifact.

    int init_process = -1;
    Result result = launchpad_launch(init_lauchpad, &init_process);

    stream_close(serial_device);

    if (result != SUCCESS)
    {
        logger_fatal("Failed to start init : %s", result_to_string(result));
    }

    int init_exit_value = 0;
    process_wait(init_process, &init_exit_value);

    logger_fatal("Init has return with code %d!", init_exit_value);
}
