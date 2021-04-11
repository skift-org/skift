/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

#include <assert.h>

#include "kernel/devices/Devices.h"
#include "kernel/devices/Driver.h"
#include "kernel/filesystem/DevicesFileSystem.h"
#include "kernel/graphics/Graphics.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/modules/Modules.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/storage/Partitions.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Tasking.h"
#include "kernel/tasking/Userspace.h"

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
