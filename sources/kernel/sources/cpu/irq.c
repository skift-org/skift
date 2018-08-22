// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "cpu/irq.h"
#include "cpu/idt.h"

extern u32 irq_vector[];
extern bool console_bypass_lock;
irq_handler_t irq_handlers[16];

void irq_setup()
{
    for(u32 i = 0; i < 16; i++)
    {
        idt_entry(32 + i, irq_vector[i], 0x08, INTGATE);
    }   
}

irq_handler_t irq_register(int index, irq_handler_t handler)
{
    if (index < 16)
    {
        irq_handlers[index] = handler;
        return handler;
    }

    return NULL;
}

esp_t irq_handler(esp_t esp, context_t context)
{
    if (irq_handlers[context.int_no] != NULL)
    {
        esp = irq_handlers[context.int_no](esp, &context);
    }
    else
    {
        // console_bypass_lock = true;
        // debug("Unhandeled IRQ %d!", context.int_no);
        // console_bypass_lock = false;
    }

    if (context.int_no >= 8)
    {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);

    // this is only use for task switching.
    return esp;
}