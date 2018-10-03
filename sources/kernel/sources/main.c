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
    log("================================================================================");
    log("Initializing cpu context...");
    setup(gdt);
    setup(pic);
    setup(idt);
    setup(isr);
    setup(irq);
}

void setup_system_context()
{
    log("================================================================================");
    log("Initializing system context...");
    setup(memory, get_kernel_end(&mbootinfo), (mbootinfo.mem_lower + mbootinfo.mem_upper) * 1024);
    setup(tasking);
    setup(filesystem);
    setup(modules, &mbootinfo);
}

void system_start()
{
    log("================================================================================");
    log("Enabling interupts, paging and atomics.");
    atomic_enable();
    sti();
    log(KERNEL_UNAME);
}

void line(int x0, int y0, int x1, int y1, int weight, uint color)
{

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        for (int xoff = -weight + 1; xoff < weight; xoff++)
        {
            for (int yoff = -weight + 1; yoff < weight; yoff++)
            {
                graphic_pixel(x0 + xoff, y0 + yoff, color);
            }
        }

        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void test()
{
    while (1)
    {
    }
}

void main(multiboot_info_t *info, s32 magic)
{
    puts("\n");
    log("================================================================================");

    memcpy(&mbootinfo, info, sizeof(multiboot_info_t));
    // Start of the boot environement ( be very carefull what you do here ;) ) //

    system_check(&mbootinfo, magic);

    graphic_early_setup(800, 600);

    setup_cpu_context();
    setup_system_context();

    setup(graphic);
    setup(mouse);
    setup(keyboard);

    // End of the boot environement //
    system_start();
    log("================================================================================");

    process_exec("application/test-app.app", 0, NULL);

    // for(size_t i = 0; i < 256; i++)
    //     thread_create(process_self(), (thread_entry_t)test, NULL, 0);


    uint oldmousex = 0;
    uint oldmousey = 0;

    while (1)
    {
        uint mousex, mousey;
        mouse_get_position(&mousex, &mousey);
        //graphic_pixel(mousex, mousey, 0xff0000);

        line(oldmousex, oldmousey, mousex, mousey, 1, mousex ^ mousey);

        oldmousex = mousex;
        oldmousey = mousey;
    }

    PANIC("The end of the main function has been reached.");
}
