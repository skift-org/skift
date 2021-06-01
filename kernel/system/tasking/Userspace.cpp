#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "system/Streams.h"
#include "system/graphics/EarlyConsole.h"
#include "system/system/System.h"
#include "system/tasking/Userspace.h"

#define DEFAULT_ENVIRONMENT "{}"

void userspace_initialize()
{
    Kernel::logln("Starting the userspace...");

    Launchpad *init_lauchpad = launchpad_create("init", "/System/Utilities/init");
    launchpad_flags(init_lauchpad, TASK_WAITABLE | TASK_USER);

    Stream *serial_device = stream_open("/Devices/serial", HJ_OPEN_WRITE | HJ_OPEN_READ);

    launchpad_handle(init_lauchpad, HANDLE(serial_device), 0);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 1);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 2);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 3);

    launchpad_environment(init_lauchpad, DEFAULT_ENVIRONMENT);

    early_console_disable(); // We disable the early console to prevent artifact.

    int init_process = -1;
    HjResult result = launchpad_launch(init_lauchpad, &init_process);

    stream_close(serial_device);

    if (result != SUCCESS)
    {
        system_panic("Failed to start init : %s", result_to_string(result));
    }

    int init_exit_value = 0;
    process_wait(init_process, &init_exit_value);

    system_panic("Init has return with code %d!", init_exit_value);
}
