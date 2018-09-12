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

#include "kernel/dev/vga.h"

#include "kernel/filesystem.h"
#include "kernel/graphic.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/version.h"

#include "sync/atomic.h"

multiboot_info_t mbootinfo;

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void main(multiboot_info_t *info, s32 magic)
{
    puts("\n");

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Invalid multiboot magic number (0x%x)!", magic);

    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);

    setup(graphic, 1024, 768);

    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);

    log(KERNEL_UNAME);

    atomic_enable();
    sti();

    while (1)
        ;

    PANIC("The end of the main function has been reached.");
}
