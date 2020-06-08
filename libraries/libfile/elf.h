#pragma once

#include <libsystem/Common.h>

#define ELF_NIDENT 16

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELF_NONE 0
#define ELF_REL 1
#define ELF_EXEC 2

typedef unsigned int elf_type_t;

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
} ELFHeader;

typedef enum
{
    ELFS_NULL = 0,     // Null section
    ELFS_PROGBITS = 1, // Program information
    ELFS_SYMTAB = 2,   // Symbol table
    ELFS_STRTAB = 3,   // String table
    ELFS_RELA = 4,     // Relocation (w/ addend)
    ELFS_NOBITS = 8,   // Not present in file
    ELFS_REL = 9,      // Relocation (no addend)
} ELFSectionType;

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
} ELFSection;

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
} ELFProgram;

typedef struct
{
    u32 name;
    u32 value;
    u32 size;
    uint8_t info;
    uint8_t other;
    u16 shndx;
} ELFSymbole;

int elf_valid(ELFHeader *header);

int elf_read_section(ELFHeader *header, ELFSection *dest, uint index);
int elf_read_program(ELFHeader *header, ELFProgram *dest, uint index);

char *elf_lookup_string(ELFHeader *header, int offset);
