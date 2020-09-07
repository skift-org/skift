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
    stream_format(log_stream, "\n");
    stream_format(log_stream, "                         _   _     _ _____ ____ _____                           \n");
    stream_format(log_stream, "                        | | | |   | | ____|  _ \\_   _|                          \n");
    stream_format(log_stream, "                        | |_| |_  | |  _| | |_) || |                            \n");
    stream_format(log_stream, "                        |  _  | |_| | |___|  _ < | |                            \n");
    stream_format(log_stream, "                        |_| |_|\\___/|_____|_| \\_\\|_|                            \n");
    stream_format(log_stream, "                                                                                \n");
    stream_format(log_stream, "\u001b[34;1m--------------------------------------------------------------------------------\e[0m\n");
    stream_format(log_stream, "                              Copyright (c) 2018-2020 The skiftOS contributors \n");
    stream_format(log_stream, "\n");

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
