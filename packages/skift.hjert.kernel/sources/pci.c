#include "kernel/pci.h"
#include "kernel/processor.h"

static u16 pci_read_word(u8 bus, u8 slot, u8 func, u8 offset) {
    u32 address;
    u32 lbus  = (u32)bus;
    u32 lslot = (u32)slot;
    u32 lfunc = (u32)func;
    u16 tmp = 0;
    address = (u32)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((u32)0x80000000));
    outw(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (u16)((inw(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

void pci_setup() {
    // TODO: cache devices (probably should be done on a thread for speed improvement)
}

// vim:set ts=4 sw=4 et:
