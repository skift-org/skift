#include <libsystem/core/Plugs.h>

#include "arch/x86/COM.h"
#include "arch/x86_64/x86_64.h"

#include "kernel/handover/Stivale1.h"

__aligned(4096) static char stack[16384] = {};

__attribute__((section(".stivalehdr"), used)) StivaleHeader header = {
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 1,
    .framebuffer_bpp = 32,
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .entry_point = 0,
};

void _start(StivaleStruct *bootloader_data)
{
    __plug_init();

    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    while (true)
    {
        hlt();
    }
}
