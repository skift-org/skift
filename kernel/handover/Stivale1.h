#pragma once

#include <libsystem/Common.h>

struct __packed Stivale1Header
{
    uint64_t stack;
    uint16_t flags;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t entry_point;
};

struct __packed Stivale1Module
{
    uint64_t begin;
    uint64_t end;
    char string[128];
    uint64_t next;
};

enum Stivale1MemoryType
{
    USABLE = 1,
    RESERVED = 2,
    ACPI_RECLAIMABLE = 3,
    NVS = 4,
    BADRAM = 5,
    KERNEL = 10,
};

struct __packed Stivale1Memory
{
    uint64_t base;   // Base of the memory section
    uint64_t length; // Length of the section
    uint32_t type;   // Type (described below)
    uint32_t unused;
};

struct __packed Stivale1Info
{
    uint64_t cmdline;
    uint64_t memory_map_addr;
    uint64_t memory_map_entries;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint64_t rsdp;
    uint64_t module_count;
    uint64_t modules;
    uint64_t epoch;
    uint64_t flags;
};
