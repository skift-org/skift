#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>

#include "arch/x86/kernel/COM.h"
#include "arch/x86/kernel/IOPort.h"
#include "arch/x86/kernel/PIC.h"
#include "arch/x86/kernel/PIT.h"
#include "arch/x86/kernel/RTC.h"
#include "arch/x86_64/kernel/GDT.h"
#include "arch/x86_64/kernel/IDT.h"
#include "arch/x86_64/kernel/x86_64.h"

#include "thirdparty/limine/stivale/stivale.h"

__aligned(4096) static char stack[16384] = {};

__attribute__((section(".stivalehdr"), used)) stivale_header header = {
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = 1,
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32,
    .entry_point = 0,
};

extern "C" void _start(void *info)
{
    __unused(info);
    __plug_init();

    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    logger_info("Hello, world!");
    gdt_initialize();
    idt_initialize();
    pic_initialize();
    pit_initialize(1);
    logger_info("Hello, world!");

    sti();

    while (true)
    {
        logger_info("Still alive!");
    }
}

void arch_disable_interrupts()
{
    cli();
}

void arch_enable_interrupts()
{
    sti();
}

void arch_halt()
{
    hlt();
}

void arch_yield()
{
    ASSERT_NOT_REACHED();
}

void arch_save_context(Task *task)
{
    __unused(task);

    ASSERT_NOT_REACHED();
}

void arch_load_context(Task *task)
{
    __unused(task);

    ASSERT_NOT_REACHED();
}

size_t arch_debug_write(const void *buffer, size_t size)
{
    return com_write(COM1, buffer, size);
}

TimeStamp arch_get_time()
{
    return rtc_now();
}

__no_return void arch_reboot()
{
    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}

__no_return void arch_shutdown()
{
    logger_warn("STUB %s", __func__);

    ASSERT_NOT_REACHED();
}

void arch_panic_dump()
{
    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}

void arch_dump_stack_frame(void *stackframe)
{
    __unused(stackframe);

    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}

void arch_backtrace()
{
    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}
