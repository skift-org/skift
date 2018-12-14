/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/elf.h>

int check_magic(elf_header_t *header)
{
    char *magic = (char *)&header->ident;

    return magic[0] == ELFMAG0 &&
           magic[1] == ELFMAG1 &&
           magic[2] == ELFMAG2 &&
           magic[3] == ELFMAG3 &&
           magic[4] == 1 && /* is 32bit */
           magic[5] == 1;
}

int elf_valid(elf_header_t *header)
{
    return check_magic(header) &&
           (header->type == ELF_REL || ELF_EXEC) &&
           header->version == 1 &&
           header->machine == 3; // 386
}

int elf_read_section(elf_header_t *header, elf_section_t *dest, uint index)
{
    if (index >= header->shnum)
        return 0;
    elf_section_t *section = (elf_section_t *)((uint)header + header->shoff + sizeof(elf_section_t) * index);

    memcpy(dest, section, sizeof(elf_section_t));

    return 1;
}

char *elf_lookup_string(elf_header_t *header, int offset)
{
    elf_section_t section;
    elf_read_section(header, &section, header->shstrndx);
    return (char *)header + section.offset + offset;
}

int elf_read_program(elf_header_t *header, elf_program_t *dest, uint index)
{
    if (index >= header->phnum)
        return 0;

    elf_program_t *section = (elf_program_t *)((uint)header + header->phoff + sizeof(elf_program_t) * index);

    memcpy(dest, section, sizeof(elf_program_t));

    return 1;
}