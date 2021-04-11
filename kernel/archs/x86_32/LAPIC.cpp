#include <libsystem/Logger.h>

#include "archs/x86/PIC.h"
#include "archs/x86_32/LAPIC.h"

constexpr int LAPIC_EOI = 0x00B0;

static volatile uint32_t *lapic = nullptr;

void lapic_found(uintptr_t address)
{
    lapic = reinterpret_cast<uint32_t *>(address);
    logger_info("LAPIC found at %08x", lapic);
}

uint32_t lapic_read(uint32_t reg)
{
    return *((volatile uint32_t *)(lapic + reg));
}

void lapic_write(uint32_t reg, uint32_t data)
{
    *((volatile uint32_t *)(lapic + reg)) = data;
}

void lapic_ack()
{
    lapic_write(LAPIC_EOI, 0);
}

void lapic_initialize()
{
    pic_disable();

    lapic_write(0xF0, lapic_read(0xF0) | 0x100);
}
