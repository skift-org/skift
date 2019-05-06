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

#include <skift/__plugs__.h>

#include <skift/atomic.h>
#include <skift/cstring.h>
#include <skift/iostream.h>
#include <skift/logger.h>
#include <skift/math.h>

#include "kernel/cpu/gdt.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/isr.h"

#include "kernel/dev/null.h"
#include "kernel/dev/random.h"
#include "kernel/dev/zero.h"

#include "kernel/console.h"
#include "kernel/filesystem.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/mouse.h"
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/tasking.h"

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void main(multiboot_info_t *info, s32 magic)
{
    __plug_init(); // init skift.karm.core glue code.

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

    sk_log(LOG_INFO, "Booting from '%s' using command lines options '%s'.", info->boot_loader_name, info->cmdline);

    /* --- Setup cpu context ------------------------------------------------ */
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);

    /* --- System context --------------------------------------------------- */
    sk_log(LOG_INFO, "Initializing system...");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(messaging);
    setup(filesystem);
    setup(modules, &mbootinfo);

    /* --- Devices ---------------------------------------------------------- */
    sk_log(LOG_INFO, "Mounting devices...");
    setup(serial);
    setup(mouse);
    setup(keyboard);
    setup(console);

    setup(null);
    setup(zero);
    setup(random);

    /* --- Finalizing System ------------------------------------------------ */
    sk_log(LOG_INFO, "Starting the userspace...");
    sk_atomic_enable();
    sti();

    printf(KERNEL_UNAME);
    printf("\n");

    /* --- Entering userspace ----------------------------------------------- */
    PROCESS init = process_exec("/bin/init", (const char *[]){"/bin/init", NULL});

    if (init)
    {
        int exitvalue = 0;
        thread_wait_process(init, &exitvalue);

        PANIC("Init has return with code %d!", exitvalue);
    }
    else
    {
        PANIC("No working init found.");
    }
}
