#include <string.h>
#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "kernel/memory.h"
#include "libelf.h"

void load_segment(uint source, uint size, uint dest, uint destsize)
{
    memory_map((uint)dest, PAGE_ALIGN(destsize) / PAGE_SIZE);
    memcpy((void*)dest, (void*)source, size);
}

typedef void (*entry_t)();

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
            load_segment((uint)buffer + program.offset, program.filesz, program.vaddr, program.memsz);
        }


        entry_t e = (entry_t)elf->entry;
        e();
    }


    free(buffer);

    return 0;
}
