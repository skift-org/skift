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

list_t *threads;
list_t *processes;

thread_t *alloc_thread(thread_entry_t entry, int user)
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

process_t *alloc_process(const char *name, int user)
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

void free_thread(thread_t *thread)
{
    free(thread->stack);
    free(thread);
}

void free_process(process_t *process)
{
    if (process->pdir != memory_kpdir())
    {
        memory_free_pdir(process->pdir);
    }

    list_free(process->threads);
    free(process);
}

void kill_thread(thread_t *thread)
{
    list_remove(thread->process->threads, thread);

    if (thread->process->threads->count == 0)
    {
        free_process(thread->process);
    }

    free_thread(thread);
}

void kill_process(process_t *process)
{
    FOREACH(i, process->threads)
    {
        free_thread((thread_t *)i->value);
    }

    free_process(process);
}

thread_t *thread_get(THREAD thread)
{

    FOREACH(i, threads)
    {
        thread_t *t = (thread_t *)i->value;

        if (t->id == thread)
            return t;
    }

    return NULL;
}

process_t *process_get(PROCESS process)
{

    FOREACH(i, processes)
    {
        process_t *p = (process_t *)i->value;

        if (p->id == process)
            return p;
    }

    return NULL;
}

/* --- Public functions ----------------------------------------------------- */

void tasking_setup()
{
    waiting = list_alloc();
    threads = list_alloc();
    processes = list_alloc();

    irq_register(0, (irq_handler_t)&shedule);
}

/* --- Thread managment ----------------------------------------------------- */

THREAD thread_self()
{
    return running->id;
}

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, int flags)
{
    UNUSED(arg);
    UNUSED(flags);

    atomic_begin();

    process_t *process = process_get(p);
    thread_t *thread = alloc_thread(entry, 0);

    list_pushback(process->threads, thread);
    thread->process = process;

    if (running)
    {
        list_pushback(waiting, thread);
    }
    else
    {
        running = thread;
    }

    thread->state = THREAD_RUNNING;

    atomic_end();

    return thread->id;
}

int thread_cancel(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_get(t);
    thread->state = THREAD_CANCELED;

    atomic_end();

    return 1;
}

void thread_exit(void *retval)
{
    UNUSED(retval);

    thread_cancel(thread_self());
    asm("int $32"); // yield

    while (1)
    {
        hlt();
    };
}

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self()
{
    return running->process->id;
}

PROCESS process_create(const char *name, int user)
{
    atomic_begin();

    process_t * process = alloc_process(name, user);
    list_pushback(processes, process);
    
    atomic_end();

    return process->id;
}

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

    if (dest >= 0x100000)
    {
        atomic_begin();

        // To avoid pagefault we need to switch page directorie.
        //page_directorie_t *pdir = running->process->pdir;
        paging_load_directorie(process->pdir);
        paging_invalidate_tlb();

        process_map(process, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
        memset((void *)dest, 0, destsz);
        memcpy((void *)dest, (void *)src, srcsz);

        paging_load_directorie(memory_kpdir());

        atomic_end();
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

void process_kill(process_t *process)
{
    atomic_begin();
    FOREACH(i, process->threads)
    {
        thread_cancel((thread_t *)i->value);
    }
    atomic_end();

    asm("int $32"); // yield
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
        thread_t *deadthread = (thread_t *)i->value;
        list_remove(waiting, (int)deadthread);
        thread_free(deadthread);
    }

    list_pop(waiting, (int *)&running);
    set_kernel_stack((uint)running->stack + STACK_SIZE);

    // Load the new context
    return running->esp;
}
