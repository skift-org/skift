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

#include <libsystem/__plugs__.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libmath/math.h>
#include <libsystem/error.h>

#include "x86/gdt.h"
#include "x86/idt.h"
#include "x86/irq.h"
#include "x86/isr.h"

#include "devices.h"
#include "filesystem.h"
#include "memory.h"
#include "modules.h"
#include "mouse.h"
#include "multiboot.h"
#include "paging.h"
#include "serial.h"
#include "system.h"
#include "tasking.h"
#include "platform.h"

multiboot_info_t mbootinfo;

void kmain(multiboot_info_t *info, uint magic)
{
    __plug_init();

    logger_setlevel(LOG_ALL);

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

    logger_log(LOG_INFO, "Booting from '%s' using command lines options '%s'.", info->boot_loader_name, info->cmdline);

    /* --- Setup cpu context ------------------------------------------------ */
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);
    setup(platform);

    /* --- System context --------------------------------------------------- */
    logger_log(LOG_INFO, "Initializing system...");
    setup(memory, &mbootinfo);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);

    /* --- Devices ---------------------------------------------------------- */
    logger_log(LOG_INFO, "Mounting devices...");
    setup(textmode);
    setup(framebuffer);
    setup(serial);
    setup(mouse);
    setup(keyboard);

    setup(proc);
    setup(null);
    setup(zero);
    setup(random);

    /* --- Finalizing System ------------------------------------------------ */
    logger_log(LOG_INFO, "Starting the userspace...");
    atomic_enable();
    sti();

    printf(KERNEL_UNAME);
    printf("\n");

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
