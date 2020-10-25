#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>

#include "architectures/x86/kernel/COM.h"
#include "architectures/x86/kernel/IOPort.h"
#include "architectures/x86/kernel/PIC.h"
#include "architectures/x86/kernel/PIT.h"
#include "architectures/x86/kernel/RTC.h"
#include "architectures/x86_64/kernel/GDT.h"
#include "architectures/x86_64/kernel/IDT.h"
#include "architectures/x86_64/kernel/x86_64.h"

#include "kernel/graphics/Graphics.h"
#include "kernel/handover/Handover.h"
#include "kernel/system/System.h"

extern "C" void arch_main(void *info, uint32_t magic)
{
    __plug_init();

    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    auto handover = handover_initialize(info, magic);

    graphic_early_initialize(handover);

    if (handover->memory_usable < 127 * 1024)
    {
        system_panic("No enoughs memory (%uKio)!", handover->memory_usable / 1024);
    }

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
