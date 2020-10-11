#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>

#include "arch/x86/kernel/COM.h"
#include "arch/x86/kernel/IOPort.h"
#include "arch/x86/kernel/RTC.h"
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

    while (true)
    {
        hlt();
    }
}

void arch_disable_interrupts()
{
    ASSERT_NOT_REACHED();
}

void arch_enable_interrupts()
{
    ASSERT_NOT_REACHED();
}

void arch_halt()
{
    ASSERT_NOT_REACHED();
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
    ASSERT_NOT_REACHED();
}

__no_return void arch_shutdown()
{
    ASSERT_NOT_REACHED();
}

void arch_panic_dump()
{
    ASSERT_NOT_REACHED();
}

void arch_dump_stack_frame(void *stackframe)
{
    __unused(stackframe);

    ASSERT_NOT_REACHED();
}

void arch_backtrace()
{
    ASSERT_NOT_REACHED();
}
