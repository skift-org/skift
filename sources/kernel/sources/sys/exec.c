#include <string.h>
#include "libelf.h"

#include "kernel/filesystem.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/tasking.h"

void load_segment(uint source, uint size, uint dest, uint destsize)
{
    memory_map((uint)dest, PAGE_ALIGN(destsize) / PAGE_SIZE);
    memset((void*)dest, 0, destsize);
    memcpy((void*)dest, (void*)source, size);
}

int exec(char *path)
{
    file_t *file = file_open(NULL, path);
    if (file == NULL)
    {
        log("EXEC: %s file not found!", path);
        return 1;
    }

    void *buffer = file_read_all(file);
    file_close(file);

    if (buffer != NULL)
    {

        ELF_header_t *elf = (ELF_header_t *)buffer;
        // log("ELF file: VALID=%d TYPE=%d ENTRY=0x%x SEG_COUNT=%i", ELF_valid(elf), elf->type, elf->entry, elf->phnum);

        ELF_program_t program;
        for (int i = 0; ELF_read_program(elf, &program, i); i++)
        {
            // log("program 0x%x(%i) -> 0x%x(%i)", program.offset, program.filesz, program.vaddr, program.memsz);
            load_segment((uint)buffer + program.offset, program.filesz, program.vaddr, program.memsz);
        }

        thread_create((thread_entry_t)elf->entry);
    }

    free(buffer);
    return 0;
}
