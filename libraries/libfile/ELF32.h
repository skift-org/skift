#pragma once

#include <libfile/ELFCommon.h>

struct __packed ELF32Header
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

struct __packed ELF32Section
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
