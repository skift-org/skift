
#include "archs/x86_64/IDT.h"

extern uintptr_t __interrupt_vector[];

IDT64Entry idt[IDT_ENTRY_COUNT] = {};

IDT64Descriptor idt_descriptor = {
    .size = sizeof(IDT64Entry) * IDT_ENTRY_COUNT,
    .offset = (uint64_t)&idt[0],
};

void idt_initialize()
{
    for (int i = 0; i < 48; i++)
    {
        idt[i] = IDT64Entry(__interrupt_vector[i], 0, INTGATE);
    }

    idt[127] = IDT64Entry(__interrupt_vector[48], 0, INTGATE);
    idt[128] = IDT64Entry(__interrupt_vector[49], 0, INTGATE | IDT_USER);

    idt_flush((uint64_t)&idt_descriptor);
}
