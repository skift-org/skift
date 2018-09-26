/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include "libelf.h"

int check_magic(ELF_header_t *header)
{
    char *magic = (char *)&header->ident;

    return magic[0] == ELFMAG0 &&
           magic[1] == ELFMAG1 &&
           magic[2] == ELFMAG2 &&
           magic[3] == ELFMAG3 &&
           magic[4] == 1 && /* is 32bit */
           magic[5] == 1;
}

int ELF_valid(ELF_header_t *header)
{
    return check_magic(header) &&
           (header->type == ELF_REL || ELF_EXEC) &&
           header->version == 1 &&
           header->machine == 3; // 386
}

int ELF_read_section(ELF_header_t *header, ELF_section_t *dest, uint index)
{
    if (index >= header->shnum)
        return 0;
    ELF_section_t *section = (ELF_section_t *)((uint)header + header->shoff + sizeof(ELF_section_t) * index);

    memcpy(dest, section, sizeof(ELF_section_t));

    return 1;
}

char *ELF_lookup_string(ELF_header_t *header, int offset)
{
    ELF_section_t section;
    ELF_read_section(header, &section, header->shstrndx);
    return (char *)header + section.offset + offset;
}

int ELF_read_program(ELF_header_t *header, ELF_program_t *dest, uint index)
{
    if (index >= header->phnum)
        return 0;
        
    ELF_program_t *section = (ELF_program_t *)((uint)header + header->phoff + sizeof(ELF_program_t) * index);

    memcpy(dest, section, sizeof(ELF_program_t));

    return 1;
}