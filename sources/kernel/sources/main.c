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
#include "kernel/time.h"
#include "kernel/version.h"

#include "sync/atomic.h"

multiboot_info_t mbootinfo;

void boot_screen(string msg)
{
    vga_clear(vga_white, vga_black);
    vga_text(36, 11, __kernel_name, vga_light_blue, vga_black);
    vga_text(40 - strlen(msg) / 2, 13, msg, vga_white, vga_black);
}

extern int __end;
uint get_kernel_end(multiboot_info_t *minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

int exec(char *path);
void main(multiboot_info_t *info, s32 magic)
{
    puts("\n");
    boot_screen("Booting...");

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
    boot_screen("Loading ramdisk...");
    setup(modules, &mbootinfo);

    boot_screen("Welcome!");
    atomic_enable();
    sti();

    thread_create(time_task);
    log(KERNEL_UNAME);

    exec("Application/test-app.app");

    log("kernel running");

    while (true)
    {
        hlt();
    };

    PANIC("The end of the main function has been reached.");
}
