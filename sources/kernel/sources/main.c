// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/irq.h"
#include "cpu/isr.h"

#include "devices/vga.h"

#include "kernel/console.h"
#include "kernel/dumping.h"
#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "kernel/memory.h"
#include "kernel/modules.h"
#include "kernel/multiboot.h"
#include "kernel/physical.h"
#include "kernel/ramdisk.h"
#include "kernel/tasking.h"
#include "kernel/time.h"
#include "kernel/version.h"
#include "kernel/virtual.h"

#include "sync/atomic.h"

multiboot_info_t mbootinfo;

void boot_screen(string msg)
{
    vga_clear(vga_white, vga_black);
    vga_text(36, 10, "skift OS", vga_light_blue, vga_black);
    vga_text(40 - strlen(msg) / 2, 14, msg, vga_white, vga_black);
}

extern int __end;
uint get_kernel_end(multiboot_info_t * minfo)
{
    return max((uint)&__end, modules_get_end(minfo));
}

void main(multiboot_info_t * info, s32 magic)
{
    console_setup(); puts("\n");

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC )
        panic("Invalid multiboot magic number (0x%x)!", magic);

    info("--- Setting up cpu tables ---");
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);

    info("--- Setting up system ---");
    setup(physical, (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(memory, get_kernel_end(&mbootinfo));
    setup(task);
    setup(filesystem);
    setup(modules, &mbootinfo);

    atomic_enable();
    sti();

    file_t* file = file_open(NULL, "Library/Header/stdio.h");
    if (file)
    {
      char* buffer = file_read_all(file);
      printf(buffer);
      file_close(file);
      free(buffer);
    }

    task_start_named(time_task, "clock");
    info(KERNEL_UNAME);

    while(true){ hlt(); };

    panic("The end of the main function has been reached.");
}
