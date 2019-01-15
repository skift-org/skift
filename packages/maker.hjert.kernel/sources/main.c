/*                  .d8888. db   dD d888888b d88888b d888888b                 */ 
/*                  88'  YP 88 ,8P'   `88'   88'     `~~88~~'                 */ 
/*                  `8bo.   88,8P      88    88ooo      88                    */ 
/*                    `Y8b. 88`8b      88    88~~~      88                    */ 
/*                  db   8D 88 `88.   .88.   88         88                    */ 
/*                  `8888Y' YP   YD Y888888P YP         YP                    */

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
#include <skift/__plugs.h>

#include "kernel/cpu/gdt.h"
#include "kernel/cpu/idt.h"
#include "kernel/cpu/irq.h"
#include "kernel/cpu/isr.h"

#include "kernel/dev/null.h"
#include "kernel/dev/random.h"
#include "kernel/dev/zero.h"

#include "kernel/console.h"
#include "kernel/filesystem.h"
#include "kernel/graphic.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/messaging.h"
#include "kernel/modules.h"
#include "kernel/mouse.h"
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/serial.h"
#include "kernel/shared_memory.h"
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
    __plug_init(); // init maker.skift.framework glue code.

    /* --- Say hello -------------------------------------------------------- */
    printf(KERNEL_UNAME);
    printf("\nCopyright © 2018-2019 MAKER.\n");
    printf("Booting...\n");

    /* --- Early operation -------------------------------------------------- */
    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
    graphic_early_setup(800, 600);

    /* --- System check ----------------------------------------------------- */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        PANIC("Wrong bootloader please use a GRUB or any multiboot2 bootloader (MBOOT_MAGIC=0x%x)!", magic);
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
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    
    setup(tasking);
    setup(messaging);
    setup(shared_memory);

    setup(filesystem);
    setup(modules, &mbootinfo);
    

    /* --- Devices ---------------------------------------------------------- */
    filesystem_mkdir("/dev");
    setup(serial);
    setup(graphic);
    setup(mouse);
    setup(keyboard);
    setup(console);

    setup(null);
    setup(zero);
    setup(random);

    filesystem_dump();

    sk_log(LOG_DEBUG, "Starting the userspace...");

    /* --- Finalizing System ------------------------------------------------ */
    sk_atomic_enable();
    sti();

    printf(KERNEL_UNAME);
    printf("\nCopyright (c) 2018-2019 MAKER.\n");
    printf("Booting...\n");

    /* --- Entering userspace ----------------------------------------------- */
    PROCESS init = process_exec("/bin/init", NULL);

    if (init)
    {
        thread_wait_process(init);
        PANIC("Init has return!");
    }
    else
    {
        PANIC("No working init found.");
    }

    PANIC("END OF KERNEL!");
}
