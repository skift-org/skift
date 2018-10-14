#pragma once
#include <skift/types.h>
#include <skift/utils.h>

#include <stdint.h>

typedef uint16_t Elf32_Half; // Unsigned half int
typedef uint32_t Elf32_Off;  // Unsigned offset
typedef uint32_t Elf32_Addr; // Unsigned address
typedef uint32_t Elf32_Word; // Unsigned int
typedef int32_t Elf32_Sword; // Signed int

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
} ELF_type_t;

typedef PACKED(struct)
{
    uint8_t ident[ELF_NIDENT];
    Elf32_Half type;
    Elf32_Half machine;
    Elf32_Word version;
    Elf32_Addr entry;
    Elf32_Off phoff;
    Elf32_Off shoff;
    Elf32_Word flags;
    Elf32_Half ehsize;
    Elf32_Half phentsize;
    Elf32_Half phnum;
    Elf32_Half shentsize;
    Elf32_Half shnum;
    Elf32_Half shstrndx;
}
ELF_header_t;

typedef enum
{
    ELFS_NULL = 0,     // Null section
    ELFS_PROGBITS = 1, // Program information
    ELFS_SYMTAB = 2,   // Symbol table
    ELFS_STRTAB = 3,   // String table
    ELFS_RELA = 4,     // Relocation (w/ addend)
    ELFS_NOBITS = 8,   // Not present in file
    ELFS_REL = 9,      // Relocation (no addend)
} ELF_scection_type_t;

typedef PACKED(struct)
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
}
ELF_section_t;

typedef struct
{
    Elf32_Word type;
    Elf32_Off  offset;
    Elf32_Addr vaddr;
    Elf32_Addr paddr;
    Elf32_Word filesz;
    Elf32_Word memsz;
    Elf32_Word flags;
    Elf32_Word align;
} ELF_program_t;

typedef struct
{
    Elf32_Word name;
    Elf32_Addr value;
    Elf32_Word size;
    uint8_t info;
    uint8_t other;
    Elf32_Half shndx;
} Elf32_Sym_t;

int ELF_valid(ELF_header_t *header);

int ELF_read_section(ELF_header_t *header, ELF_section_t *dest, uint index);
int ELF_read_program(ELF_header_t *header, ELF_program_t *dest, uint index);

char *ELF_lookup_string(ELF_header_t *header, int offset);
