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

int PID = 0;
int TID = 0;

list_t *threads;
list_t *processes;

thread_t *alloc_thread(thread_entry_t entry, int user)
{
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    memset(thread, 0, sizeof(thread_t));

    thread->id = TID++;

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

    log("Thread with ID=%d allocated.", thread->id);

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

    log("Process '%s' with ID=%d allocated.", process->name, process->id);

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

PROCESS kernel_process;
THREAD kernel_thread;

thread_t *running;
list_t *waiting;

esp_t shedule(esp_t esp, context_t *context);

void tasking_setup()
{
    waiting = list_alloc();
    threads = list_alloc();
    processes = list_alloc();


    kernel_process = process_create("kernel", 0);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);

    
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

    log("Thread with ID=%d is running.", thread->id);

    return thread->id;
}

int thread_cancel(THREAD t)
{
    atomic_begin();

    thread_t * thread = thread_get(t);
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

    process_t *process = alloc_process(name, user);
    list_pushback(processes, process);

    atomic_end();

    log("Process '%s' with ID=%d is running.", process->name, process->id);

    return process->id;
}

void process_cancel(PROCESS p)
{
    atomic_begin();

    process_t * process = process_get(p);

    FOREACH(i, process->threads)
    {
        thread_cancel(((thread_t*)i->value)->id);
    }

    atomic_end();
}

void process_exit(int code)
{
    UNUSED(code);

    atomic_begin();

    PROCESS p = process_self();
    process_t *process = process_get(p);

    process->exitcode = code;

    if (p != kernel_process)
    {
        process_cancel(p);
    }
    else
    {
        log("Kernel try to commit suicide!");
    }

    atomic_end();
}

int process_map(PROCESS p, uint addr, uint count)
{
    return memory_map(process_get(p)->pdir, addr, count, 1);
}

int process_unmap(PROCESS p, uint addr, uint count)
{
    return memory_unmap(process_get(p)->pdir, addr, count);
}

/* --- Sheduler ------------------------------------------------------------- */

extern uint ticks;

esp_t shedule(esp_t esp, context_t *context)
{
    ticks++;

    UNUSED(context);

    // Save the old context
    running->esp = esp;

    list_pop(waiting, (void*)&running);
    set_kernel_stack((uint)running->stack + STACK_SIZE);

    // Load the new context
    return running->esp;
}
