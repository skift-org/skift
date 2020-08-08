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

struct __packed ELFHeader
{
    uint8_t ident[ELF_NIDENT];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
};

enum ELFSectionType
{
    ELFS_NULL = 0,     // Null section
    ELFS_PROGBITS = 1, // Program information
    ELFS_SYMTAB = 2,   // Symbol table
    ELFS_STRTAB = 3,   // String table
    ELFS_RELA = 4,     // Relocation (w/ addend)
    ELFS_NOBITS = 8,   // Not present in file
    ELFS_REL = 9,      // Relocation (no addend)
};

struct __packed ELFSection
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
};

struct ELFProgram
{
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
};

struct ELFSymbole
{
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
};

int elf_valid(ELFHeader *header);

int elf_read_section(ELFHeader *header, ELFSection *dest, uint index);
int elf_read_program(ELFHeader *header, ELFProgram *dest, uint index);

char *elf_lookup_string(ELFHeader *header, int offset);
