/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* tasking.c: Sheduling, messaging, sharedmemory and sheduling                */

/*
 * TODO:
 * - The name of some function need refactor.
 * - Isolate user space process in ring 3
 * - Add a process/thread garbage colector
 * - Move the sheduler in his own file.
 * - Allow to pass parameters to thread and then return values
 * - Add priority to the round robine sheduler
 */

#include <stdlib.h>
#include <string.h>
#include <skift/atomic.h>
#include <skift/elf.h>
#include <skift/logger.h>

#include "kernel/cpu/gdt.h"
#include "kernel/cpu/irq.h"
#include "kernel/filesystem.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/processor.h"
#include "kernel/system.h"
#include "kernel/limits.h"

#include "kernel/tasking.h"
#include "kernel/messaging.h"
#include "kernel/shared_memory.h"

int PID = 1;
int TID = 1;

uint ticks = 0;
list_t *processes;

list_t *threads;
list_t *threads_bystates[THREADSTATE_COUNT];

/* --- thread table --------------------------------------------------------- */

thread_t idle;
void idle_code()
{
    while (1)
    {
        hlt();
    }
}

void collect_and_free_thread(void)
{
    list_t *thread_to_free = list();

    // Search for thread with a canceled parent process.
    FOREACH(i, threads_bystates[THREADSTATE_CANCELED])
    {
        thread_t *thread = i->value;

        if (thread->process->state == PROCESS_CANCELED)
        {
            list_pushback(thread_to_free, thread);
        }
    }

    // Cleanup all of those dead threads.
    FOREACH(i, thread_to_free)
    {
        thread_t *thread = i->value;
        int thread_id = thread->id;

        sk_log(LOG_DEBUG, "free'ing thread %d", thread_id);

        // All those guys have a ptr to us we need to remove them.
        thread_setstate(thread, THREADSTATE_NONE);
        list_remove(threads, thread);
        list_remove(thread->process->threads, thread);

        // Now no one should still have a ptr to us we can die in peace.
        free(thread);

        sk_log(LOG_DEBUG, "Thread %d free'd.", thread_id);
    }

    list_delete(thread_to_free);
}

void collect_and_free_process(void)
{
    // Ho god, this is going to be hard :/
    list_t* process_to_free = list();

    list_delete(process_to_free);
}

void garbage_colector()
{
    while (true)
    {
        thread_sleep(100); // We don't do this really often.

        // sk_log(LOG_DEBUG, "Garbage collect begin %d !", ticks);

        sk_atomic_begin();
        collect_and_free_thread();
        collect_and_free_process();
        sk_atomic_end();

        // sk_log(LOG_DEBUG, "Garbage collect end!");
    }
}

void thread_setup(void)
{
    threads = list();

    for (int i = 0; i < THREADSTATE_COUNT; i++)
    {
        threads_bystates[i] = list();
    }
}

thread_t *thread()
{
    thread_t *thread = MALLOC(thread_t);

    if (thread == NULL)
    {
        PANIC("Failed to allocated a new thread.");
    }

    memset(thread, 0, sizeof(thread_t));

    thread->id = TID++;
    thread->state = THREADSTATE_NONE;

    list_pushback(threads, thread);

    return thread;
}

thread_t *thread_getbyid(int id)
{
    FOREACH(i, threads)
    {
        thread_t *thread = i->value;

        if (thread->id == id)
            return thread;
    }

    return NULL;
}

bool shortest_sleep_first(void *left, void *right)
{
    return ((thread_t *)left)->wait.time.wakeuptick < ((thread_t *)right)->wait.time.wakeuptick;
}

void thread_setstate(thread_t *thread, thread_state_t state)
{
    if (thread->state != THREADSTATE_NONE)
    {
        list_remove(threads_bystates[thread->state], thread);
    }

    thread->state = state;

    if (thread->state != THREADSTATE_NONE)
    {
        if (thread->state == THREADSTATE_WAIT_TIME)
        {
            list_insert_sorted(threads_bystates[THREADSTATE_WAIT_TIME], thread, shortest_sleep_first);
        }
        else
        {
            list_push(threads_bystates[thread->state], thread);
        }
    }
}

void thread_init(thread_t *t, thread_entry_t entry, bool user)
{
    t->entry = entry;
    t->user = user;
    // setup the stack
    memset(t->stack, 0, MAX_THREAD_STACKSIZE);
    t->sp = (reg32_t)(&t->stack[0] + MAX_THREAD_STACKSIZE - 1);
}

uint thread_stack_push(thread_t *t, const void *value, uint size)
{
    t->sp -= size;
    memcpy((void *)t->sp, value, size);

    return t->sp;
}

void thread_attach_process(thread_t *t, process_t *p)
{
    if (t->process == NULL)
    {
        list_pushback(p->threads, t);
        t->process = p;
    }
    else
    {
        PANIC("Trying to attaching thread %d of process %d to process %d.", t->id, t->process->id, p->id);
    }
}

void thread_ready(thread_t *t)
{
    processor_context_t ctx;

    ctx.eflags = 0x202;
    ctx.eip = (reg32_t)t->entry;
    ctx.ebp = ((reg32_t)t->stack + MAX_THREAD_STACKSIZE);

    // TODO: userspace thread
    ctx.cs = 0x08;
    ctx.ds = 0x10;
    ctx.es = 0x10;
    ctx.fs = 0x10;
    ctx.gs = 0x10;

    thread_stack_push(t, &ctx, sizeof(ctx));

    thread_setstate(t, THREADSTATE_RUNNING);
}

process_t *alloc_process(const char *name, bool user)
{
    process_t *process = MALLOC(process_t);

    if (process == NULL)
    {
        PANIC("Failed to allocated a new process.");
    }

    process->id = PID++;

    strncpy(process->name, name, MAX_PROCESS_NAMESIZE);
    process->user = user;
    process->threads = list();
    process->inbox = list();
    process->shared = list();

    if (user)
    {
        process->pdir = memory_alloc_pdir();
    }
    else
    {
        process->pdir = memory_kpdir();
    }

    sk_log(LOG_FINE, "Process '%s' with ID=%d allocated.", process->name, process->id);

    return process;
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

thread_t *running = NULL;

reg32_t shedule(reg32_t esp, processor_context_t *context);

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    sk_log(LOG_DEBUG, "Timer frequency is %dhz.", hz);
}

// define in boot.s
extern u32 __stack_bottom;

void tasking_setup()
{
    running = NULL;

    processes = list();

    thread_setup();

    kernel_process = process_create("kernel", false);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);
    thread_create(kernel_process, garbage_colector, NULL, false);

    // Setup the idle task
    idle.id = -1;
    thread_init(&idle, idle_code, false);
    thread_attach_process(&idle, process_get(kernel_process));
    thread_ready(&idle);

    timer_set_frequency(100);
    irq_register(0, (irq_handler_t)&shedule);
}

/* --- Thread managment ----------------------------------------------------- */

void thread_yield()
{
    asm("int $32");
}

#pragma GCC push_options
#pragma GCC optimize("O0") // Look like gcc like to break this functions XD

void thread_hold()
{
    while (running->state != THREADSTATE_RUNNING)
    {
        hlt();
    }
}

#pragma GCC pop_options

THREAD thread_self()
{
    if (running == NULL)
        return 0;

    return running->id;
}

thread_t *thread_running()
{
    return running;
}

// Create the main thread of a user application
THREAD thread_create_mainthread(PROCESS p, thread_entry_t entry, const char **argv)
{
    sk_atomic_begin();

    process_t *process = process_get(p);
    thread_t *t = thread();

    thread_init(t, entry, true);
    thread_attach_process(t, process);

    // WIP: push arguments
    uint argv_list[MAX_PROCESS_ARGV];

    int argc;
    for (argc = 0; argv[argc] && argc < MAX_PROCESS_ARGV; argc++)
    {
        argv_list[argc] = thread_stack_push(t, argv[argc], strlen(argv[argc]) + 1);
    }

    uint argv_list_ref = thread_stack_push(t, &argv_list, sizeof(argv_list));

    thread_stack_push(t, &argv_list_ref, sizeof(argv_list_ref));
    thread_stack_push(t, &argc, sizeof(argc));

    thread_ready(t);

    sk_atomic_end();

    return t->id;
}

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, bool user)
{
    sk_atomic_begin();

    process_t *process = process_get(p);
    thread_t *t = thread();

    thread_init(t, entry, user);
    thread_attach_process(t, process);
    thread_stack_push(t, &arg, sizeof(arg));
    thread_ready(t);

    if (running == NULL)
    {
        running = t;
    }

    sk_atomic_end();

    return t->id;
}

void thread_sleep(int time)
{
    ATOMIC({
        running->wait.time.wakeuptick = ticks + time;
        thread_setstate(running, THREADSTATE_WAIT_TIME);
    });

    thread_hold();
}

void thread_wakeup(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL && thread->state == THREADSTATE_WAIT_TIME)
    {
        thread_setstate(running, THREADSTATE_RUNNING);
    }

    sk_atomic_end();
}

bool thread_wait_thread(THREAD t, int *exitvalue)
{
    sk_atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL)
    {
        if (thread->state == THREADSTATE_CANCELED)
        {
            if (exitvalue != NULL)
            {
                *exitvalue = thread->exitvalue;
            }

            sk_atomic_end();
        }
        else
        {
            running->wait.thread.thread_handle = t;
            thread_setstate(running, THREADSTATE_WAIT_THREAD);

            sk_atomic_end();
            thread_hold();

            if (exitvalue != NULL)
            {
                *exitvalue = running->wait.thread.exitvalue;
            }
        }

        return true;
    }
    else
    {
        sk_atomic_end();
        return false;
    }
}

bool thread_wait_process(PROCESS p, int *exitvalue)
{
    sk_atomic_begin();

    process_t *process = process_get(p);

    if (process != NULL)
    {
        if (process->state == PROCESS_CANCELED || process->state == PROCESS_CANCELING)
        {
            if (exitvalue != NULL)
            {
                *exitvalue = process->exitvalue;
            }

            sk_atomic_end();
        }
        else
        {
            running->wait.process.process_handle = p;
            thread_setstate(running, THREADSTATE_WAIT_PROCESS);

            sk_atomic_end();
            thread_hold();

            if (exitvalue != NULL)
            {
                *exitvalue = running->wait.process.exitvalue;
            }
        }

        return true;
    }
    else
    {
        sk_atomic_end();
        return false;
    }
}

bool thread_cancel(THREAD t, int exitvalue)
{
    sk_atomic_begin();

    thread_t *thread = thread_getbyid(t);

    if (thread != NULL && thread->process == running->process)
    {
        // Set the new thread state
        thread->exitvalue = exitvalue;
        thread_setstate(running, THREADSTATE_CANCELED);

        sk_log(LOG_DEBUG, "Thread(%d) got canceled.", t);

        // Wake up waiting threads
        FOREACH(i, thread->process->threads)
        {
            thread_t *waitthread = i->value;

            if (waitthread->state == THREADSTATE_WAIT_THREAD &&
                waitthread->wait.thread.thread_handle == t)
            {
                waitthread->wait.thread.exitvalue = exitvalue;
                thread_setstate(waitthread, THREADSTATE_RUNNING);

                sk_log(LOG_DEBUG, "Thread %d finish waiting thread %d.", waitthread->id, thread->id);
            }
        }

        sk_atomic_end();
        return true;
    }
    else
    {
        sk_atomic_end();
        return false;
    }
}

void thread_exit(int exitvalue)
{
    thread_cancel(running->id, exitvalue);

    while (1)
        hlt();
}

void thread_dump_all()
{
    sk_atomic_begin();

    printf("\n\tCurrent thread:");
    thread_dump(running);

    printf("\n");

    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        thread_t *t = i->value;
        if (t != running && t->state != THREADSTATE_NONE)
            thread_dump(t);
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
    }

    sk_atomic_end();
}

static char *THREAD_STATES[] =
    {
        "RUNNING",
        "SLEEP",
        "WAIT(thread)",
        "WAIT(process)",
        "WAIT(message)",
        "CANCELED",
};

void thread_dump(thread_t *t)
{
    sk_atomic_begin();

    printf("\n\t- ID=%d PROC=('%s', %d) %s", t->id, t->process->name, t->process->id, THREAD_STATES[t->state]);

    sk_atomic_end();
}

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self()
{
    if (running == NULL)
        return -1;

    return running->process->id;
}

process_t *process_running()
{
    if (running == NULL)
        return NULL;

    return running->process;
}

PROCESS process_create(const char *name, bool user)
{
    process_t *process;

    ATOMIC({
        process = alloc_process(name, user);
        list_pushback(processes, process);
    });

    sk_log(LOG_FINE, "Process '%s' with ID=%d and PDIR=%x is running.", process->name, process->id, process->pdir);

    return process->id;
}

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    sk_log(LOG_DEBUG, "Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

    if (dest >= 0x100000)
    {
        sk_atomic_begin();

        // To avoid pagefault we need to switch page directorie.
        page_directorie_t *pdir = running->process->pdir;

        paging_load_directorie(process->pdir);
        process_map(process->id, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
        memset((void *)dest, 0, destsz);
        memcpy((void *)dest, (void *)src, srcsz);

        paging_load_directorie(pdir);

        sk_atomic_end();
    }
    else
    {
        sk_log(LOG_WARNING, "Elf segment ignored, not in user memory!");
    }
}

PROCESS process_exec(const char *path, const char **argv)
{
    stream_t *s = filesystem_open(path, OPENOPT_READ);
    if (s == NULL)
    {
        sk_log(LOG_WARNING, "'%s' file not found, exec failed!", path);
        return 0;
    }

    file_stat_t stat;
    filesystem_fstat(s, &stat);

    if (stat.type != FSFILE)
    {
        sk_log(LOG_WARNING, "'%s' is not a file, exec failed!", path);
        return 0;
    }

    void *buffer = filesystem_readall(s);
    filesystem_close(s);

    if (buffer == NULL)
    {
        sk_log(LOG_WARNING, "Failed to read from '%s', exec failed!", path);
        return 0;
    }

    PROCESS p = process_create(path, true);

    elf_header_t *elf = (elf_header_t *)buffer;

    elf_program_t program;

    for (int i = 0; elf_read_program(elf, &program, i); i++)
    {
        load_elfseg(process_get(p), (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
    }

    thread_create_mainthread(p, (thread_entry_t)elf->entry, argv);

    free(buffer);

    return p;
}

bool process_cancel(PROCESS p, int exitvalue)
{
    sk_atomic_begin();

    if (p != kernel_process)
    {
        process_t *process = process_get(p);

        // Set our new process state
        process->state = PROCESS_CANCELED;
        process->exitvalue = exitvalue;

        sk_log(LOG_DEBUG, "Process '%s' ID=%d canceled!", process->name, process->id);

        // Wake up waiting threads
        FOREACH(i, threads)
        {
            thread_t *thread = i->value;

            if (thread->state == THREADSTATE_WAIT_PROCESS && thread->wait.process.process_handle == p)
            {
                thread->wait.process.exitvalue = exitvalue;
                thread_setstate(thread, THREADSTATE_RUNNING);
                sk_log(LOG_DEBUG, "Thread %d finish waiting process %d.", thread->id, p);
            }
        }

        // Cancel childs threads.
        FOREACH(i, process->threads)
        {
            thread_t *thread = (thread_t *)i->value;
            thread_cancel(thread->id, 0);
        }

        sk_atomic_end();
        return true;
    }
    else
    {
        process_t *process = process_get(process_self());
        sk_log(LOG_WARNING, "Process '%s' ID=%d tried to commit murder on the kernel!", process->name, process->id);

        sk_atomic_end();
        return false;
    }
}

void process_exit(int exitvalue)
{
    PROCESS self = process_self();

    if (self != kernel_process)
    {
        process_cancel(self, exitvalue);

        // Hang
        while (1)
            hlt();
    }
    else
    {
        PANIC("Kernel try to commit suicide!");
    }
}

int process_map(PROCESS p, uint addr, uint count)
{
    return memory_map(process_get(p)->pdir, addr, count, 1);
}

int process_unmap(PROCESS p, uint addr, uint count)
{
    return memory_unmap(process_get(p)->pdir, addr, count);
}

uint process_alloc(uint count)
{
    uint addr = memory_alloc(running->process->pdir, count, 1);
    return addr;
}

void process_free(uint addr, uint count)
{
    return memory_free(running->process->pdir, addr, count, 1);
}

/* --- Sheduler ------------------------------------------------------------- */

void wakeup_sleeping_threads(void)
{
    if (!list_empty(threads_bystates[THREADSTATE_WAIT_TIME]))
    {
        thread_t *t;

        do
        {
            list_peek(threads_bystates[THREADSTATE_WAIT_TIME], (void **)&t);

            if (t->wait.time.wakeuptick <= ticks)
            {
                thread_setstate(t, THREADSTATE_RUNNING);
                // sk_log(LOG_DEBUG, "Thread %d wake up!", t->id);
            }

        } while (t->stack == THREADSTATE_RUNNING);
    }
}

bool is_context_switch = false;
reg32_t shedule(reg32_t sp, processor_context_t *context)
{
    UNUSED(context);
    is_context_switch = true;

    // Save the old context
    running->sp = sp;

    // Update the system ticks
    ticks++;
    wakeup_sleeping_threads();

    // Get the next thread
    if (list_pop(threads_bystates[THREADSTATE_RUNNING], (void **)&running))
    {
        list_pushback(threads_bystates[THREADSTATE_RUNNING], running);
    }
    else
    {
        running = &idle;
    }

    // Restore the context
    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    is_context_switch = false;

    return running->sp;
}
