/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

#include "arch/Arch.h"
#include "arch/x86/Interrupts.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/graphics/Graphics.h"
#include "kernel/modules/Modules.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Tasking.h"
#include "kernel/tasking/Userspace.h"

void system_main(Multiboot *multiboot)
{
    logger_info("Framebuffer at %08x %d %d", multiboot->framebuffer_addr, multiboot->framebuffer_width, multiboot->framebuffer_height);

    system_initialize();
    memory_initialize(multiboot);
    scheduler_initialize();
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
    graphic_initialize(multiboot);
    userspace_initialize();

    ASSERT_NOT_REACHED();
}
