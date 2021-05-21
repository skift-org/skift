/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

#include <assert.h>

#include "system/devices/Devices.h"
#include "system/devices/Driver.h"
#include "system/graphics/Graphics.h"
#include "system/interrupts/Interupts.h"
#include "system/modules/Modules.h"
#include "system/scheduling/Scheduler.h"
#include "system/storage/Partitions.h"
#include "system/system/System.h"
#include "system/tasking/Tasking.h"
#include "system/tasking/Userspace.h"

#include "system/Streams.h"

#include "devfs/DevicesFileSystem.h"
#include "devfs/DevicesInfo.h"
#include "procfs/ProcessInfo.h"

static void splash_screen()
{
    stream_format(log_stream, "\n");
    stream_format(log_stream, "                         _   _     _ _____ ____ _____                           \n");
    stream_format(log_stream, "                        | | | |   | | ____|  _ \\_   _|                          \n");
    stream_format(log_stream, "                        | |_| |_  | |  _| | |_) || |                            \n");
    stream_format(log_stream, "                        |  _  | |_| | |___|  _ < | |                            \n");
    stream_format(log_stream, "                        |_| |_|\\___/|_____|_| \\_\\|_|                            \n");
    stream_format(log_stream, "                                                                                \n");
    stream_format(log_stream, "\u001b[34;1m--------------------------------------------------------------------------------\e[0m\n");
    stream_format(log_stream, "                              Copyright (c) 2018-2021 The skiftOS contributors \n");
    stream_format(log_stream, "\n");
}

void system_main(Handover *handover)
{
    Kernel::logln("Hello, world from libio!");

    splash_screen();
    system_initialize();
    memory_initialize(handover);
    scheduler_initialize();
    tasking_initialize();
    interrupts_initialize();
    modules_initialize(handover);
    driver_initialize();
    device_initialize();
    partitions_initialize();
    process_info_initialize();
    device_info_initialize();
    devices_filesystem_initialize();
    graphic_initialize(handover);
    userspace_initialize();

    ASSERT_NOT_REACHED();
}
