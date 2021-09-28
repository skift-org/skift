#pragma once

#include <libutils/Prelude.h>

#define ELF_MAG0 0x7F
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'

#define ELF_IDENT_COUNT 16
#define ELF_IDENT_MAG0 0
#define ELF_IDENT_MAG1 1
#define ELF_IDENT_MAG2 2
#define ELF_IDENT_MAG3 3

#define ELF_IDENT_CLASS 4
#define ELF_IDENT_CLASS_NONE 0
#define ELF_IDENT_CLASS_32 1
#define ELF_IDENT_CLASS_64 2

#define ELF_IDENT_DATA 5
#define ELF_IDENT_DATA_NONE 0
#define ELF_IDENT_DATA_LSB 1
#define ELF_IDENT_DATA_MSB 2

#define ELF_IDENT_VERSION 6
#define ELF_IDENT_OSABI 7
#define ELF_IDENT_ABIVERSION 8
#define ELF_IDENT_PAD 9

#define ELF_ETYPE_NONE 0
#define ELF_ETYPE_REL 1
#define ELF_ETYPE_EXEC 2
#define ELF_ETYPE_DYN 3
#define ELF_ETYPE_CORE 4

#define ELF_MACHINE_NONE 0
#define ELF_MACHINE_SPARC 2
#define ELF_MACHINE_386 3
#define ELF_MACHINE_SPARC32PLUS 18
#define ELF_MACHINE_SPARCV9 43
#define ELF_MACHINE_AMD64 62

#define ELF_FLAG_SPARC_EXT_MASK 0xffff00
#define ELF_FLAG_SPARC_32PLUS 0x000100
#define ELF_FLAG_SPARC_SUN_US1 0x000200
#define ELF_FLAG_SPARC_HAL_R1 0x000400
#define ELF_FLAG_SPARC_SUN_US3 0x000800
#define ELF_FLAG_SPARCV9_MM 0x3
#define ELF_FLAG_SPARCV9_TSO 0x0
#define ELF_FLAG_SPARCV9_PSO 0x1
#define ELF_FLAG_SPARCV9_RMO 0x2

#define ELF_PROGRAM_X 0x1
#define ELF_PROGRAM_W 0x2
#define ELF_PROGRAM_R 0x4

#define ELF_SECTION_TYPE_NULL 0
#define ELF_SECTION_TYPE_PROGBITS 1
#define ELF_SECTION_TYPE_SYMTAB 2
#define ELF_SECTION_TYPE_STRTAB 3
#define ELF_SECTION_TYPE_RELA 4
#define ELF_SECTION_TYPE_HASH 5
#define ELF_SECTION_TYPE_DYNAMIC 6
#define ELF_SECTION_TYPE_NOTE 7
#define ELF_SECTION_TYPE_NOBITS 8
#define ELF_SECTION_TYPE_REL 9
#define ELF_SECTION_TYPE_SHLIB 10
#define ELF_SECTION_TYPE_DYNSYM 11
#define ELF_SECTION_TYPE_COUNT 12

/* --- ELF32 ---------------------------------------------------------------- */

struct PACKED ELF32Header
{
    uint8_t ident[ELF_IDENT_COUNT];
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

    bool valid()
    {
        char *magic = (char *)&ident;

        bool is_magic_ok = magic[ELF_IDENT_MAG0] == ELF_MAG0 &&
                           magic[ELF_IDENT_MAG1] == ELF_MAG1 &&
                           magic[ELF_IDENT_MAG2] == ELF_MAG2 &&
                           magic[ELF_IDENT_MAG3] == ELF_MAG3 &&
                           magic[ELF_IDENT_CLASS] == ELF_IDENT_CLASS_32 &&
                           magic[ELF_IDENT_DATA] == ELF_IDENT_DATA_LSB;

        return is_magic_ok && (type == ELF_ETYPE_REL || ELF_ETYPE_EXEC) &&
               version == 1 && machine == ELF_MACHINE_386;
    }
};

struct PACKED ELF32Section
{
    uint32_t name;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addralign;
    uint32_t entsize;
};

struct ELF32Program
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

struct ELF32Symbole
{
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
};

struct ELF32
{
    using Header = ELF32Header;
    using Section = ELF32Section;
    using Program = ELF32Program;
    using Symbole = ELF32Symbole;
};

/* --- ELF64 ---------------------------------------------------------------- */

struct PACKED ELF64Header
{
    uint8_t ident[ELF_IDENT_COUNT];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;

    bool valid()
    {
        char *magic = (char *)&ident;

        bool is_magic_ok = magic[ELF_IDENT_MAG0] == ELF_MAG0 &&
                           magic[ELF_IDENT_MAG1] == ELF_MAG1 &&
                           magic[ELF_IDENT_MAG2] == ELF_MAG2 &&
                           magic[ELF_IDENT_MAG3] == ELF_MAG3 &&
                           magic[ELF_IDENT_CLASS] == ELF_IDENT_CLASS_64 &&
                           magic[ELF_IDENT_DATA] == ELF_IDENT_DATA_LSB;

        return is_magic_ok && (type == ELF_ETYPE_REL || ELF_ETYPE_EXEC) &&
               version == 1 && machine == ELF_MACHINE_AMD64;
    }
};

struct PACKED ELF64Section
{
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
};

struct PACKED ELF64Program
{
    uint32_t type;
    uint32_t flags;

    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};

struct PACKED ELF64Symbole
{
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
};

struct ELF64
{
    using Header = ELF64Header;
    using Section = ELF64Section;
    using Program = ELF64Program;
    using Symbole = ELF64Symbole;
};
