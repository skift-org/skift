/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* elf.c: in memory elf file parser.                                          */

#include <libsystem/cstring.h>
#include <libfile/elf.h>

int check_magic(elf_header_t *header)
{
    char *magic = (char *)&header->ident;

    return magic[0] == ELFMAG0 &&
           magic[1] == ELFMAG1 &&
           magic[2] == ELFMAG2 &&
           magic[3] == ELFMAG3 &&
           magic[4] == 1       &&
           magic[5] == 1;
}

int elf_valid(elf_header_t *header)
{
    return check_magic(header) &&
           (header->type == ELF_REL || ELF_EXEC) &&
           header->version == 1 &&
           header->machine == 3; // x86
}
