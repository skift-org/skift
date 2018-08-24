#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "libelf.h"

int exec(char *path)
{
    file_t *file = file_open(NULL, path);
    if (file == NULL)
    {
        debug("%s file not found!", path);
        return 1;
    }
    void *buffer = file_read_all(file);
    file_close(file);

    if (buffer != NULL)
    {
        ELF_header_t *elf = (ELF_header_t *)buffer;
        debug("ELF file: VALID=%d TYPE=%d ENTRY=0x%x SEC=%i", ELF_valid(elf), elf->type, elf->entry, elf->shnum);

        ELF_section_t section;
        for (int i = 0; ELF_read_section(elf, &section, i); i++)
        {
            debug("section %s@%x", ELF_lookup_string(elf, section.name), (uint)elf + section.offset);
        }

        ELF_program_t program;
        for (int i = 0; ELF_read_program(elf, &program, i); i++)
        {
            debug("program 0x%x(%i) -> 0x%x(%i)", program.offset, program.filesz, program.vaddr, program.memsz);
        }
    }

    free(buffer);

    return 0;
}
