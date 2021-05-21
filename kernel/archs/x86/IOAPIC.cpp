#include "system/Streams.h"

#include "archs/x86/IOAPIC.h"

namespace Arch::x86
{

static volatile uint32_t *ioapic = nullptr;

void ioapic_found(uintptr_t address)
{
    ioapic = reinterpret_cast<uint32_t *>(address);
    Kernel::logln("IOAPIC found at {08x}", ioapic);
}

uint32_t ioapic_read(uint32_t reg)
{
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}

void ioapic_write(uint32_t reg, uint32_t value)
{
    ioapic[0] = (reg & 0xff);
    ioapic[4] = value;
}

} // namespace Arch::x86
