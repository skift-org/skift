#include "arch/x86/x86.h"

#include "arch/x86/COM.h"
#include "arch/x86/FPU.h"
#include "arch/x86/GDT.h"
#include "arch/x86/IDT.h"
#include "arch/x86/PIC.h"
#include "arch/x86/PIT.h"
#include "arch/x86/RTC.h"

void arch_initialize(void)
{
    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    gdt_initialize();
    idt_initialize();
    pic_initialize();
    fpu_initialize();
    pit_initialize(1000);
}

void arch_disable_interupts(void)
{
    cli();
}

void arch_enable_interupts(void)
{
    sti();
}

void arch_halt(void)
{
    hlt();
}

void arch_save_context(Task *task)
{
    fpu_save_context(task);
}

void arch_load_context(Task *task)
{
    fpu_load_context(task);
}

size_t arch_debug_write(const void *buffer, size_t size)
{
    return com_write(COM1, buffer, size);
}

TimeStamp arch_get_time(void)
{
    return rtc_now();
}
