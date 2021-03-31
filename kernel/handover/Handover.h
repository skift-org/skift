#pragma once

#include "kernel/graphics/PixelFormat.h"
#include "kernel/memory/MemoryRange.h"

#define HANDOVER_BOOTLOADER_NAME_SIZE 64
#define HANDOVER_COMMAND_LINE_SIZE 256
#define HANDOVER_MEMORY_MAP_SIZE 64
#define HANDOVER_MODULES_SIZE 16

enum MemoryMapEntryType
{
    MEMORY_MAP_ENTRY_AVAILABLE,
    MEMORY_MAP_ENTRY_RESERVED,
    MEMORY_MAP_ENTRY_ACPI_RECLAIMABLE,
    MEMORY_MAP_ENTRY_NVS,
    MEMORY_MAP_ENTRY_BADRAM,
    MEMORY_MAP_ENTRY_KERNEL
};

struct MemoryMapEntry
{
    MemoryRange range{};
    MemoryMapEntryType type;
};

struct Module
{
    MemoryRange range;
    char command_line[HANDOVER_COMMAND_LINE_SIZE];
};

struct Handover
{
    char bootloader[HANDOVER_BOOTLOADER_NAME_SIZE];
    char command_line[HANDOVER_COMMAND_LINE_SIZE];

    size_t memory_usable;

    MemoryMapEntry memory_map[HANDOVER_MEMORY_MAP_SIZE];
    size_t memory_map_size;

    Module modules[HANDOVER_MODULES_SIZE];
    size_t modules_size;

    uintptr_t framebuffer_addr;
    size_t framebuffer_width;
    size_t framebuffer_height;
    size_t framebuffer_pitch;
    size_t framebuffer_bpp;

    uintptr_t acpi_rsdp_address;
};

Handover *handover();

void handover_assert(uint32_t magic);

Handover *handover_initialize(void *header, uint32_t magic);

bool is_multiboot2(uint32_t magic);

bool is_stivale2(uint32_t magic);

void multiboot2_parse_header(Handover *handover, void *header_ptr);

void stivale2_parse_header(Handover *handover, void *header_ptr);
