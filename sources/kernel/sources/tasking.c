#include <stdlib.h>
#include <string.h>
#include "sync/atomic.h"

#include "cpu/cpu.h"
#include "cpu/gdt.h"
#include "cpu/irq.h"

#include "kernel/memory.h"
#include "kernel/system.h"
#include "kernel/tasking.h"
#include "kernel/virtual.h"
#include "kernel/physical.h"

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

process_t *process_alloc(bool user)
{
    process_t *process = (process_t *)malloc(sizeof(process_t));

    process->id = PID++;
    process->user = user;
    process->threads = list_alloc();

    if (user)
    {
        process->pdir = memory_construct_memory_space();
    }
    else
    {
        process->pdir = get_kernel_page_dir();
    }

    return process;
}

void process_free(process_t *process)
{
    atomic_begin();

    memory_detroy_memory_space(process->pdir);
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

/* --- Process --- */

process_t *process_exec(const char *path, int argc, char **argv)
{
    STUB(path, argc, argv);
    return NULL;
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

uint process_map(process_t *process, uint addr, uint count)
{
    void *mem = physical_alloc_contiguous(count);

    for (size_t i = 0; i < count; i++)
    {
        uint virtual = addr + PAGE_SIZE * i;
        if (virtual2physical(process->pdir, virtual) == 0)
        {
            virtual_map(process->pdir, virtual, (uint)mem + PAGE_SIZE * i, true);
        }
    }

    return (uint)mem;
}

uint process_unmap(process_t *process, uint addr, uint count)
{
    for (size_t i = 0; i < count; i++)
    {
        uint virtual = addr + PAGE_SIZE * i;
        uint physical = virtual2physical(process->pdir, virtual);

        if (physical)
        {
            physical_free((void*)physical);
            virtual_unmap(process->pdir, virtual);
        }
    }

    return 1;
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
