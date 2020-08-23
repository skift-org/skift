#include "arch/x86/LAPIC.h"

static volatile uint32_t *lapic = nullptr;

void lapic_found(uintptr_t address)
{
    lapic = reinterpret_cast<uint32_t *>(address);
}

uint32_t lapic_read(uint32_t reg)
{
    return *((volatile uint32_t *)(lapic + reg));
}

void lapic_write(uint32_t reg, uint32_t data)
{
    *((volatile uint32_t *)(lapic + reg)) = data;
}
