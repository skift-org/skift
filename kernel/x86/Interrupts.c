/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Syscalls.h"
#include "kernel/x86/IDT.h"
#include "kernel/x86/Interrupts.h"
#include "kernel/x86/PIC.h"

extern uintptr_t __interrupt_vector[];

IDTEntry idt[IDT_ENTRY_COUNT];

IDTDescriptor idt_descriptor = {
    .offset = (u32)&idt[0],
    .size = sizeof(IDTEntry) * IDT_ENTRY_COUNT,
};

void interrupts_initialize(void)
{
    logger_info("Remaping the PIC...");

    pic_remap();

    logger_info("Populating the IDT...");

    for (int i = 0; i < 32; i++)
    {
        idt[i] = IDT_ENTRY(__interrupt_vector[i], 0x08, INTGATE);
    }

    for (int i = 32; i < 48; i++)
    {
        idt[i] = IDT_ENTRY(__interrupt_vector[i], 0x08, INTGATE);
    }

    idt[128] = IDT_ENTRY(__interrupt_vector[48], 0x08, TRAPGATE);

    logger_info("Flushing the IDT...");
    idt_flush((u32)&idt_descriptor);

    dispatcher_initialize();

    atomic_enable();
    sti();
}

static const char *_exception_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void interrupts_dump_stackframe(InterruptStackFrame *stackframe)
{
    printf("\tCS=%04x DS=%04x ES=%04x FS=%04x GS=%04x\n", stackframe->cs, stackframe->ds, stackframe->es, stackframe->fs, stackframe->gs);
    printf("\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", stackframe->eax, stackframe->ebx, stackframe->ecx, stackframe->edx);
    printf("\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", stackframe->edi, stackframe->esi, stackframe->ebp, stackframe->esp);
    printf("\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", stackframe->intno, stackframe->err, stackframe->eip, stackframe->eflags);

    printf("\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", CR0(), CR2(), CR3(), CR4());
}

uint32_t interrupts_handler(uintptr_t esp, InterruptStackFrame stackframe)
{
    if (stackframe.intno < 32)
    {
        CPANIC(&stackframe,
               "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !",
               _exception_messages[stackframe.intno],
               stackframe.intno,
               stackframe.err);
    }
    else if (stackframe.intno < 48)
    {
        atomic_disable();

        int irq = stackframe.intno - 32;

        if (irq == 0)
        {
            esp = shedule(esp);
        }
        else
        {
            dispatcher_dispatch(irq);
        }

        atomic_enable();
    }
    else if (stackframe.intno == 128)
    {
        stackframe.eax = task_do_syscall(stackframe.eax, stackframe.ebx, stackframe.ecx, stackframe.edx, stackframe.esi, stackframe.edi);
    }

    pic_ack(stackframe.intno);

    return esp;
}
