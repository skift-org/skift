/*                       _   _     _ _____ ____ _____                         */
/*                      | | | |   | | ____|  _ \_   _|                        */
/*                      | |_| |_  | |  _| | |_) || |                          */
/*                      |  _  | |_| | |___|  _ < | |                          */
/*                      |_| |_|\___/|_____|_| \_\|_|                          */
/*                                                                            */

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* main.c : the entry point of the kernel.                                    */

/*
 * TODO:
 * - ADD support for kernel command line options.
 */

#include <libmath/math.h>
#include <libsystem/__plugs__.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#include "x86/gdt.h"
#include "x86/idt.h"
#include "x86/irq.h"
#include "x86/isr.h"

#include "clock.h"
#include "device/Device.h"
#include "filesystem/Filesystem.h"
#include "memory.h"
#include "modules.h"
#include "mouse.h"
#include "multiboot.h"
#include "paging.h"
#include "platform.h"
#include "serial.h"
#include "system.h"
#include "tasking.h"

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
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);
    setup(platform);

    /* --- System context --------------------------------------------------- */
    logger_info("Initializing system...");
    setup(memory, &mbootinfo);
    filesystem_initialize();
    setup(tasking);

    /* --- Finalizing System ------------------------------------------------ */
    atomic_enable();
    sti();

    /* --- Devices ---------------------------------------------------------- */
    setup(modules, &mbootinfo);

    logger_info("Mounting devices...");

    null_initialize();
    zero_initialize();
    random_initialize();

    if (!framebuffer_initialize(info))
    {
        textmode_initialize();
    }

    serial_initialize();

    mouse_initialize();
    keyboard_initialize();

    logger_info("Starting the userspace...");

    /* --- Entering userspace ----------------------------------------------- */
    int init = task_exec("/bin/init", (const char *[]){"/bin/init", NULL});

    if (init < 0)
    {
        PANIC("Failled to start init : %s", error_to_string(-init));
    }
    else
    {
        int exitvalue = 0;
        task_wait(init, &exitvalue);

        PANIC("Init has return with code %d!", exitvalue);
    }
}
