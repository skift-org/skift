/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <libelf.h>

#include "sync/atomic.h"

#include "kernel/logger.h"
#include "kernel/tasking.h"
#include "kernel/filesystem.h"
#include "kernel/memory.h"

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

    if (dest >= 0x100000)
    {
        // To avoid pagefault we need to switch page directorie.
        //page_directorie_t *pdir = running->process->pdir;
        paging_load_directorie(process->pdir);
        paging_invalidate_tlb();

        process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
        memset((void *)dest, 0, destsz);
        memcpy((void *)dest, (void *)src, srcsz);

        paging_load_directorie(memory_kpdir());
    }
    else
    {
        log("Elf segment ignored, not in user memory!");
    }
}

PROCESS process_exec(const char *path, int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    file_t *fp = file_open(NULL, path);

    if (!fp)
    {
        log("EXEC: %s file not found, exec failed!", path);
        return NULL;
    }

    process_t *process = process_alloc(path, 1);

    void *buffer = file_read_all(fp);
    file_close(fp);

    ELF_header_t *elf = (ELF_header_t *)buffer;

    log("ELF file: VALID=%d TYPE=%d ENTRY=0x%x SEG_COUNT=%i", ELF_valid(elf), elf->type, elf->entry, elf->phnum);

    ELF_program_t program;

    atomic_begin();

    for (int i = 0; ELF_read_program(elf, &program, i); i++)
    {
        printf("\n");
        load_elfseg(process, (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
    }

    atomic_end();

    paging_load_directorie(process->pdir);

    free(buffer);

    return process;
}