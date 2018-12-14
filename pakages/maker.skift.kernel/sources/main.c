/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <skift/atomic.h>

#include "kernel/cpu/gdt.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/isr.h"

#include "kernel/filesystem.h"
#include "kernel/graphic.h"
#include "kernel/keyboard.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/mouse.h"
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/version.h"

#define LINE \
    "================================================================================"

#define THIN_LINE \
    "--------------------------------------------------------------------------------"

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void main(multiboot_info_t *info, s32 magic)
{
    /* --- Say hello -------------------------------------------------------- */
    log(KERNEL_UNAME);
    log("Copyright © 2018-2019 MAKER.");

    log("Booting...");

    /* --- Early operation -------------------------------------------------- */
    log(THIN_LINE);
    log("Early setup...");
    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
    graphic_early_setup(800, 600);

    /* --- System check ----------------------------------------------------- */
    log(THIN_LINE);
    log("System check...");
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Wrong boot loader please use a GRUB or any multiboot2 bootloader (MBOOT_MAGIC=0x%x)!", magic);

    if ((info->mem_lower + info->mem_upper) < 255 * 1024)
    {
        PANIC("No enought memory (%dkib)!", info->mem_lower + info->mem_upper);
    }
    log("Tests passed!");

    /* --- Setup cpu context ------------------------------------------------ */
    log(THIN_LINE);
    log("Initializing cpu context...");
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);

    /* --- System context --------------------------------------------------- */
    log(THIN_LINE);
    log("Initializing system context...");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);

    /* --- Devices ---------------------------------------------------------- */
    log(THIN_LINE);
    log("Initializing device context...");
    setup(graphic);
    setup(mouse);
    setup(keyboard);

    /* --- Finalizing System ------------------------------------------------ */
    log(THIN_LINE);
    log("Enabling interupts, paging and atomics.");
    sk_atomic_enable();
    sti();
    log(KERNEL_UNAME);

    /* --- Entering userspace ----------------------------------------------- */
    log(LINE);
    log("Entering userland...");
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
