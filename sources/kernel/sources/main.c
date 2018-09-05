/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/irq.h"
#include "cpu/isr.h"

#include "devices/vga.h"

#include "kernel/filesystem.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/multiboot.h"
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

int k = 0;

void *test(void *arg)
{
    UNUSED(arg);

    atomic_begin();
    int myk = k++;
    atomic_end();

    //printf("my k is %d\n", myk);

    while (true)
    {
        for (size_t i = 0; i < 256; i++)
        {

            for (size_t j = 0; j < 25; j++)
            {
                vga_cell(myk, j, vga_entry(j + (myk * 25), vga_white + myk, vga_red + myk));
            }
        }
    };

    return NULL;
}

void main(multiboot_info_t *info, s32 magic)
{
    puts("\n");

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        PANIC("Invalid multiboot magic number (0x%x)!", magic);

    log("--- Setting up cpu tables ---");
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);

    log("--- Setting up system ---");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);

    log(KERNEL_UNAME);
 
    atomic_enable();
    sti();

    for (size_t i = 0; i < 80; i++)
        thread_create(process_self(), test, NULL, 0);

    while(1);

    PANIC("The end of the main function has been reached.");
}
