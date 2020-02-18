/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* main.c : the entry point of the kernel.                                    */

#include <libmath/math.h>
#include <libsystem/Result.h>
#include <libsystem/__plugs__.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>

#include <thirdparty/multiboot/Multiboot.h>

#include "kernel/clock.h"
#include "kernel/device/Device.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/paging.h"
#include "kernel/platform.h"
#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/x86/Interrupts.h"
#include "kernel/x86/gdt.h"

static multiboot_info_t mbootinfo = {0};
static TimeStamp boot_timestamp = 0;

ElapsedTime system_get_uptime(void)
{
    return clock_now() - boot_timestamp;
}

void kmain(multiboot_info_t *info, uint magic)
{
    __plug_init();

    boot_timestamp = clock_now();

    logger_level(LOGGER_TRACE);

    /* --- Early operation -------------------------------------------------- */

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));

    /* --- System check ----------------------------------------------------- */

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC("Wrong bootloader please use a GRUB or any multiboot2 bootloader\n\tMagic number: 0x%08x != 0x%08x !", magic, MULTIBOOT_BOOTLOADER_MAGIC);
    }

    if ((info->mem_lower + info->mem_upper) < 255 * 1024)
    {
        PANIC("No enought memory (%dkib)!", info->mem_lower + info->mem_upper);
    }

    logger_info(KERNEL_UNAME);
    logger_info("Bootloader: %s", info->boot_loader_name);
    logger_info("Command lines: %s", info->cmdline);

    switch (info->framebuffer_type)
    {
    case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
        logger_info("Framebuffer type: EGA_TEXT");
        break;
    case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
        logger_info("Framebuffer type: INDEXED");
        break;
    case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
        logger_info("Framebuffer type: RGB");

        logger_info("Framebuffer adress: 0x%08x", info->framebuffer_addr);

        logger_info("Framebuffer pixel format: %d:%dR %d:%dG %d:%dB",
                    info->framebuffer_red_masize,
                    info->framebuffer_red_field_position,

                    info->framebuffer_green_masize,
                    info->framebuffer_green_field_position,

                    info->framebuffer_blue_masize,
                    info->framebuffer_blue_field_position);
        break;

    default:
        logger_error("Framebuffer type: INVALID");
        break;
    }

    logger_info("Framebuffer: %dx%dx%d", info->framebuffer_width, info->framebuffer_height, info->framebuffer_bpp);

    /* --- Setup cpu context ------------------------------------------------ */

    setup(gdt);
    setup(platform);

    /* --- System context --------------------------------------------------- */

    logger_info("Initializing system...");
    setup(memory, &mbootinfo);
    tasking_initialize();

    interrupts_initialize();

    /* --- Devices ---------------------------------------------------------- */

    logger_info("Mounting devices...");

    filesystem_initialize();

    setup(modules, &mbootinfo);
    null_initialize();
    zero_initialize();
    random_initialize();
    serial_initialize();
    mouse_initialize();
    keyboard_initialize();

    if (!framebuffer_initialize(info))
    {
        textmode_initialize();
    }

    /* --- Entering userspace ----------------------------------------------- */

    logger_info("Starting the userspace...");

    Launchpad *init_lauchpad = launchpad_create("init", "/bin/init");

    Stream *keyboard_device = stream_open("/dev/keyboard", OPEN_READ);
    Stream *serial_device = stream_open("/dev/serial", OPEN_WRITE);

    launchpad_handle(init_lauchpad, HANDLE(keyboard_device), 0);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 1);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 2);
    launchpad_handle(init_lauchpad, HANDLE(serial_device), 3);

    int init_process = launchpad_launch(init_lauchpad);

    stream_close(keyboard_device);
    stream_close(serial_device);

    if (init_process < 0)
    {
        PANIC("Failled to start init : %s", result_to_string((Result)-init_process));
    }

    int init_exitvalue = 0;
    task_wait(init_process, &init_exitvalue);

    PANIC("Init has return with code %d!", init_exitvalue);
}
