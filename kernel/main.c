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
#include <skift/error.h>

#include <hjert/cpu/gdt.h>
#include <hjert/cpu/idt.h>
#include <hjert/cpu/irq.h>
#include <hjert/cpu/isr.h>

#include <hjert/dev/null.h>
#include <hjert/dev/random.h>
#include <hjert/dev/zero.h>

#include <hjert/console.h>
#include <hjert/filesystem.h>
#include <hjert/keyboard.h>
#include <hjert/memory.h>
#include <hjert/modules.h>
#include <hjert/mouse.h>
#include <hjert/multiboot.h>
#include <hjert/paging.h>
#include <hjert/serial.h>
#include <hjert/system.h>
#include <hjert/tasking.h>

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void kmain(multiboot_info_t *info, s32 magic)
{
    __plug_init(); // init skift.karm.core glue code.

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

    /* --- System context --------------------------------------------------- */
    logger_log(LOG_INFO, "Initializing system...");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(messaging);
    setup(filesystem);
    setup(modules, &mbootinfo);

    /* --- Devices ---------------------------------------------------------- */
    logger_log(LOG_INFO, "Mounting devices...");
    setup(console);
    setup(serial);
    setup(mouse);
    setup(keyboard);

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
