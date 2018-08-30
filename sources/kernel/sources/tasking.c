/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
#include <string.h>
#include "libelf.h"
#include "sync/atomic.h"

#include "cpu/cpu.h"
#include "cpu/gdt.h"
#include "cpu/irq.h"

#include "kernel/filesystem.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/system.h"
#include "kernel/tasking.h"

esp_t shedule(esp_t esp, context_t *context);

extern uint ticks;

int PID = 0;
int TID = 0;

thread_t *running;
list_t *waiting;
list_t *dead;

list_t *process;
process_t *kernel_process;

thread_t *thread_alloc(thread_entry_t entry, int user)
{
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    memset(thread, 0, sizeof(thread_t));

    atomic_begin();
    thread->id = TID++;
    atomic_end();

    thread->stack = malloc(STACK_SIZE);
    thread->entry = entry;

    thread->esp = ((uint)thread->stack + STACK_SIZE);
    thread->esp -= sizeof(context_t);

    context_t *context = (context_t *)thread->esp;

    context->eflags = 0x202;
    context->eip = (u32)entry;

    if (user)
    {
        context->cs = 0x18;
        context->ds = 0x20;
        context->es = 0x20;
        context->fs = 0x20;
        context->gs = 0x20;
    }
    else
    {
        context->cs = 0x08;
        context->ds = 0x10;
        context->es = 0x10;
        context->fs = 0x10;
        context->gs = 0x10;
    }

    return thread;
}

void thread_free(thread_t *thread)
{
    free(thread->stack);
    free(thread);
}

process_t *process_alloc(const char *name, int user)
{
    process_t *process = (process_t *)malloc(sizeof(process_t));

    process->id = PID++;
    strncpy(process->name, name, PROCNAME_SIZE);
    process->user = user;
    process->threads = list_alloc();

    if (user)
    {
        process->pdir = memory_alloc_pdir();
    }
    else
    {
        process->pdir = memory_kpdir();
    }

    return process;
}

void process_free(process_t *process)
{
    atomic_begin();

    if (process->pdir != memory_kpdir())
    {
        memory_free_pdir(process->pdir);
    }

    list_free(process->threads);
    free(process);

    atomic_end();
}

/* --- Public Functions ----------------------------------------------------- */

void tasking_setup()
{
    waiting = list_alloc();
    dead = list_alloc();
    process = list_alloc();

    // Create the kernel task.
    thread_create(NULL);

    irq_register(0, (irq_handler_t)&shedule);
}

thread_t *thread_create(thread_entry_t entry)
{
    thread_t *thread = thread_alloc(entry, 0);

    atomic_begin();

    if (running)
    {
        list_pushback(waiting, (int)thread);
    }
    else
    {
        running = thread;
    }

    atomic_end();

    return thread;
}

int thread_cancel(thread_t *thread)
{
    atomic_begin();

    if (thread == NULL)
        return 0;

    if (!list_containe(dead, (int)thread))
    {
        list_pushback(dead, (int)thread);
    }

    atomic_end();

    return 1;
}

void thread_exit()
{
    thread_cancel(thread_self());

    asm("int $32"); // yield

    while (1)
    {
        hlt();
    };
}

thread_t *thread_self()
{
    return running;
}

/* --- Process -------------------------------------------------------------- */

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    atomic_begin();

    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

    // To avoid pagefault we need to switch page directorie.
    //page_directorie_t *pdir = running->process->pdir;
    paging_load_directorie(process->pdir);
    paging_invalidate_tlb();

    log("ok");
    process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
    log("ok");
    memset((void *)dest, 0, destsz);
    log("ok");
    memcpy((void *)dest, (void *)src, srcsz);
    log("switching page directorie");

    paging_load_directorie(memory_kpdir());

    atomic_end();
}

process_t *process_exec(const char *path, int argc, char **argv)
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
    
    // memory_dump(process->pdir);

    ELF_program_t program;
    for (int i = 0; ELF_read_program(elf, &program, i); i++)
    {
        printf("\n");
        load_elfseg(process, (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
    }

    thread_entry_t entry = (thread_entry_t)elf->entry;

    paging_load_directorie(process->pdir);
    entry();

    free(buffer);

    return process;
}

void process_kill(process_t *process)
{
    atomic_begin();
    FOREACH(i, process->threads)
    {
        thread_cancel((thread_t *)i);
    }
    atomic_end();

    asm("int $32"); // yield
}

process_t *process_self()
{
    return running->process;
}

void process_exit(int code)
{
    UNUSED(code);

    process_t *self = process_self();
    if (self != kernel_process)
    {
        process_kill(self);
    }
    else
    {
        PANIC("Kernel commit suicide!");
    }
}

int process_map(process_t *process, uint addr, uint count)
{
    return memory_map(process->pdir, addr, count, 1);
}

int process_unmap(process_t *process, uint addr, uint count)
{
    return memory_unmap(process->pdir, addr, count);
}

esp_t shedule(esp_t esp, context_t *context)
{
    ticks++;

    UNUSED(context);

    // Save the old context
    running->esp = esp;

    if (!list_containe(dead, (int)running))
    {
        list_pushback(waiting, (int)running);
    }

    FOREACH(i, dead)
    {
        thread_t *deadthread = (thread_t *)i;
        list_remove(waiting, (int)deadthread);
        thread_free(deadthread);
    }

    list_pop(waiting, (int *)&running);
    set_kernel_stack((uint)running->stack + STACK_SIZE);

    // Load the new context
    return running->esp;
}
