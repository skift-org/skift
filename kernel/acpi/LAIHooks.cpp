#include <libsystem/Logger.h>
#include <thirdparty/lai/include/lai/host.h>

#include "arch/x86/x86.h"
#include "kernel/bus/PCI.h"
#include "kernel/memory/Memory.h"
#include "kernel/memory/Virtual.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"

void laihost_log(int level, const char *msg)
{
    if (level == LAI_DEBUG_LOG)
    {
        logger_debug("%s", msg);
    }

    if (level == LAI_WARN_LOG)
    {
        logger_warn("%s", msg);
    }
}

void laihost_panic(const char *msg)
{
    system_panic("ACPI: %s", msg);
}

void *laihost_malloc(size_t size)
{
    return malloc(size);
}

void *laihost_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void laihost_free(void *ptr)
{
    free(ptr);
}

void *laihost_map(size_t address, size_t count)
{
    return (void *)virtual_alloc(memory_kpdir(), MemoryRange{address, count * PAGE_SIZE}, MEMORY_NONE).base;
}

void laihost_unmap(void *pointer, size_t count)
{
    virtual_free(memory_kpdir(), MemoryRange{(uintptr_t)pointer, count * PAGE_SIZE});
}

void *laihost_scan(char *sig, size_t index)
{
    __unused(sig);
    __unused(index);

    //TODO

    return nullptr;
}

void laihost_outb(uint16_t port, uint8_t val)
{
    out8(port, val);
}

void laihost_outw(uint16_t port, uint16_t val)
{
    out16(port, val);
}

void laihost_outd(uint16_t port, uint32_t val)
{
    out32(port, val);
}

uint8_t laihost_inb(uint16_t port)
{
    return in8(port);
}

uint16_t laihost_inw(uint16_t port)
{
    return in16(port);
}

uint32_t laihost_ind(uint16_t port)
{
    return in32(port);
}

void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);
    __unused(val);
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);
    __unused(val);
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);
    __unused(val);
}

uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);

    return 0;
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);

    return 0;
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    __unused(seg);
    __unused(bus);
    __unused(slot);
    __unused(fun);
    __unused(offset);

    return 0;
}

void laihost_sleep(uint64_t ms)
{
    task_sleep(scheduler_running(), ms);
}

void laihost_handle_amldebug(lai_variable_t *var)
{
    __unused(var);
}
