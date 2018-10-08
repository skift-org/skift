/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "sync/atomic.h"

#define LINE \
"================================================================================"

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void system_check(multiboot_info_t *info, s32 magic)
{
    log("System check...");
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Invalid multiboot magic number (0x%x)!", magic);

    if (info->mem_lower + info->mem_upper > 256 * 1024)
    {
        PANIC("No enought memory!");
    }
}

void setup_cpu_context()
{
    log(LINE);
    log("Initializing cpu context...");
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);
}

void setup_system_context()
{
    log(LINE);
    log("Initializing system context...");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);
}

void system_start()
{
    log(LINE);
    log("Enabling interupts, paging and atomics.");
    atomic_enable();
    sti();
    log(KERNEL_UNAME);
}

void main(multiboot_info_t *info, s32 magic)
{
    puts("\n");
    log(LINE);

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
    system_check(&mbootinfo, magic);

    graphic_early_setup(800, 600);

    setup_cpu_context();
    setup_system_context();

    setup(graphic);
    setup(mouse);
    setup(keyboard);

    // End of the boot environement //
    system_start();
    log(LINE);

    PROCESS init = process_exec("application/test-app.app", 0, NULL);
    thread_waitproc(init);
    log("Hello, world for the kernel!");
    
    while(1) hlt();

    PANIC("The end of the main function has been reached.");
}
