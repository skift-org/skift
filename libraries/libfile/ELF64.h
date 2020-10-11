#pragma once

#include <libfile/ELFCommon.h>

struct __packed ELF64Header
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

struct __packed ELF64Section
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

struct __packed ELF64Program
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

struct __packed ELF64Symbole
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
