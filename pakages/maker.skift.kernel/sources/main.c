/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* main.c : the entry point of the kernel.                                    */

/*
 * TODO:
 * - ADD support for kernel command line options.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <skift/atomic.h>
#include <skift/logger.h>

#include "kernel/cpu/gdt.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/isr.h"

#include "kernel/filesystem.h"
#include "kernel/graphic.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/mouse.h"
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/version.h"

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void main(multiboot_info_t *info, s32 magic)
{
    /* --- Say hello -------------------------------------------------------- */
    printf(KERNEL_UNAME);
    printf("\nCopyright © 2018-2019 MAKER.\n");
    printf("Booting...\n");

    /* --- Early operation -------------------------------------------------- */
    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
    graphic_early_setup(800, 600);

    /* --- System check ----------------------------------------------------- */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Wrong boot loader please use a GRUB or any multiboot2 bootloader (MBOOT_MAGIC=0x%x)!", magic);

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
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);

    /* --- Devices ---------------------------------------------------------- */
    setup(graphic);
    setup(mouse);
    setup(keyboard);

    /* --- Finalizing System ------------------------------------------------ */
    sk_atomic_enable();
    sti();

    shared_memory_create(1);

    /* --- Entering userspace ----------------------------------------------- */
    PROCESS session = process_exec("app/maker.hideo.session", NULL);

    if (session)
    {
        thread_waitproc(session);
        PANIC("Init has return!");
    }
    else
    {
        PANIC("No working init found.");
    }

    PANIC("END OF KERNEL!");
}
