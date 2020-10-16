
#include "architectures/x86_64/kernel/IDT.h"

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
        idt[i] = IDT64Entry(__interrupt_vector[i], 0, INTGATE);
    }

    idt[127] = IDT64Entry(__interrupt_vector[48], 1, INTGATE);

    idt_flush((uint64_t)&idt_descriptor);
}
