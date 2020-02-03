#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define ELF_NIDENT 16

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

typedef enum
{
    ELF_NONE = 0, // Unkown Type
    ELF_REL = 1,  // Relocatable File
    ELF_EXEC = 2  // Executable File
} elf_type_t;

typedef struct __packed
{
    u8 ident[ELF_NIDENT];
    u16 type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoff;
    u32 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shstrndx;
} elf_header_t;

typedef enum
{
    ELFS_NULL = 0,     // Null section
    ELFS_PROGBITS = 1, // Program information
    ELFS_SYMTAB = 2,   // Symbol table
    ELFS_STRTAB = 3,   // String table
    ELFS_RELA = 4,     // Relocation (w/ addend)
    ELFS_NOBITS = 8,   // Not present in file
    ELFS_REL = 9,      // Relocation (no addend)
} elf_section_type_t;

typedef struct __packed
{
    uint name;
    uint type;
    uint flags;
    uint addr;
    uint offset;
    uint size;
    uint link;
    uint info;
    uint addralign;
    uint entsize;
} elf_section_t;

typedef struct
{
    u32 type;
    u32 offset;
    u32 vaddr;
    u32 paddr;
    u32 filesz;
    u32 memsz;
    u32 flags;
    u32 align;
} elf_program_t;

typedef struct
{
    u32 name;
    u32 value;
    u32 size;
    uint8_t info;
    uint8_t other;
    u16 shndx;
} elf_sym_t;

int elf_valid(elf_header_t *header);

int elf_read_section(elf_header_t *header, elf_section_t *dest, uint index);
int elf_read_program(elf_header_t *header, elf_program_t *dest, uint index);

char *elf_lookup_string(elf_header_t *header, int offset);
