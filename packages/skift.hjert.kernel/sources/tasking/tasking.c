/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
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
#include "kernel/sheduler.h"

int PID = 1;

uint ticks = 0;
list_t *processes;

void garbage_colector();

/* --- thread table --------------------------------------------------------- */

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

void tasking_setup()
{
    running = NULL;

    processes = list();

    thread_setup();

    kernel_process = process_create("kernel", false);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);
    thread_create(kernel_process, garbage_colector, NULL, false);

    sheduler_setup(thread_getbyid(kernel_thread), kernel_process);
}

/* --- Thread managment ----------------------------------------------------- */

// Create the main thread of a user application
THREAD thread_create_mainthread(PROCESS p, thread_entry_t entry, const char **argv)
{
    sk_log(LOG_DEBUG, "Creating process %d main thread with eip@%08x.", p, entry);

    sk_atomic_begin();

    process_t *process = process_get(p);
    thread_t *t = thread();

    thread_setentry(t, entry, true);
    thread_attach_to_process(t, process);

    // WIP: push arguments
    uint argv_list[MAX_PROCESS_ARGV] = {0};

    int argc;
    for (argc = 0; argv[argc] && argc < MAX_PROCESS_ARGV; argc++)
    {
        argv_list[argc] = thread_stack_push(t, argv[argc], strlen(argv[argc]) + 1);
    }

    uint argv_list_ref = thread_stack_push(t, &argv_list, sizeof(argv_list));

    thread_stack_push(t, &argv_list_ref, sizeof(argv_list_ref));
    thread_stack_push(t, &argc, sizeof(argc));

    thread_setready(t);
    thread_setstate(t, THREADSTATE_RUNNING);

    sk_atomic_end();

    return t->id;
}

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, bool user)
{
    sk_atomic_begin();

    process_t *process = process_get(p);
    thread_t *t = thread();

    thread_setentry(t, entry, user);
    thread_attach_to_process(t, process);
    thread_stack_push(t, &arg, sizeof(arg));
    thread_setready(t);
    thread_setstate(t, THREADSTATE_RUNNING);

    sk_atomic_end();

    return t->id;
}

void thread_sleep(int time)
{
    ATOMIC({
        running->wait.time.wakeuptick = ticks + time;
        thread_setstate(running, THREADSTATE_WAIT_TIME);
    });

    sheduler_yield();
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
            sheduler_yield();

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
            sheduler_yield();

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
    // Check if the file existe and open the file.
    stream_t *s = filesystem_open(ROOT, path, OPENOPT_READ);
    if (s == NULL)
    {
        sk_log(LOG_WARNING, "'%s' file not found, exec failed!", path);
        return 0;
    }

    // Check if the file isn't a directory.
    file_stat_t stat;
    filesystem_fstat(s, &stat);

    if (stat.type != FSFILE)
    {
        sk_log(LOG_WARNING, "'%s' is not a file, exec failed!", path);
        return 0;
    }

    // Read the content of the file.
    void *buffer = filesystem_readall(s);
    filesystem_close(s);

    if (buffer == NULL)
    {
        sk_log(LOG_WARNING, "Failed to read from '%s', exec failed!", path);
        return 0;
    }

    // Decode the elf file header.
    elf_header_t *elf = (elf_header_t *)buffer;

    if (!elf_valid(elf))
    {
        sk_log(LOG_WARNING, "Invalid elf program!", path);
        return 0;
    }

    // Create the process and load the executable.
    PROCESS p = process_create(path, true);

    elf_program_t program;

    for (int i = 0; elf_read_program(elf, &program, i); i++)
    {
        load_elfseg(process_get(p), (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
    }

    sk_log(LOG_DEBUG, "Executable loaded, creating main thread...");

    thread_create_mainthread(p, (thread_entry_t)elf->entry, argv);

    free(buffer);

    sk_log(LOG_DEBUG, "Process created, back to caller..");
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
        FOREACH(i, thread_bystate(THREADSTATE_WAIT_PROCESS))
        {
            thread_t *thread = i->value;

            if (thread->wait.process.process_handle == p)
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