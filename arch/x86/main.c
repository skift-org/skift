/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

/* main.c : the entry point of the kernel.                                    */

#include <libsystem/Assert.h>
#include <libsystem/BuildInfo.h>
#include <libsystem/Logger.h>
#include <libsystem/__plugs__.h>

#include "arch/Arch.h"

#include "kernel/clock.h"
#include "kernel/devices/Devices.h"
#include "kernel/modules/Modules.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Userspace.h"

static TimeStamp boot_timestamp = 0;

ElapsedTime system_get_uptime(void)
{
    return clock_now() - boot_timestamp;
}

void kmain(void *info, uint magic)
{
    __plug_init();
    boot_timestamp = clock_now();
    logger_level(LOGGER_TRACE);
    logger_info("hjert - " __BUILD_GITREF__);

    Multiboot *multiboot = multiboot_initialize(info, magic);

    if (multiboot->memory_usable < 127 * 1024)
    {
        PANIC("No enought memory (%uKio)!", multiboot->memory_usable / 1024);
    }

    logger_info("Initializing system...");
    arch_initialize();
    memory_initialize(multiboot);
    tasking_initialize();
    interrupts_initialize();
    filesystem_initialize();
    modules_initialize(multiboot);
    device_initialize();
    null_initialize();
    zero_initialize();
    random_initialize();
    serial_initialize();
    mouse_initialize();
    keyboard_initialize();
    process_info_initialize();
    device_info_initialize();
    // textmode_initialize();
    userspace_initialize();

    ASSERT_NOT_REACHED();
}
