/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

/* main.c : the entry point of the kernel.                                    */

#include <libsystem/Assert.h>

#include "arch/Arch.h"
#include "arch/x86/Interrupts.h"

#include "kernel/devices/Devices.h"
#include "kernel/modules/Modules.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/system/System.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Userspace.h"

void system_main(Multiboot *multiboot)
{
    system_initialize();
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
