#pragma once

#include <libsystem/Common.h>
#include <libsystem/Logger.h>

#define INTGATE 0x8e
#define TRAPGATE 0xeF

#define IDT_USER 0b01100000

#define IDT_ENTRY_COUNT 256

struct PACKED IDT64Descriptor
{
    uint16_t size;
    uint64_t offset;
};

struct PACKED IDT64Entry
{
    uint16_t offset_low16;
    uint16_t cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid16;
    uint32_t offset_high32;
    uint32_t zero;

    constexpr IDT64Entry() : IDT64Entry(0, 0, 0) {}

    constexpr IDT64Entry(uintptr_t offset, uint8_t ist, uint8_t attr)
        : offset_low16(offset & 0xffff),
          cs(0x08),
          ist(ist),
          attributes(attr),
          offset_mid16((offset >> 16) & 0xffff),
          offset_high32((offset >> 32) & 0xffffffff),
          zero(0)
    {
    }
};

extern "C" void idt_flush(uint64_t);

void idt_initialize();
