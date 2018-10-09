/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
#include <string.h>

#include "libelf.h"
#include "sync/atomic.h"

#include "kernel/cpu/cpu.h"
#include "kernel/cpu/gdt.h"
#include "kernel/cpu/irq.h"

#include "kernel/filesystem.h"
#include "kernel/logger.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/system.h"
#include "kernel/tasking.h"

int PID = 0;
int TID = 0;

uint ticks = 0;

list_t *threads;
list_t *processes;

#define THREAD_FREE(thread)                                    \
    {                                                          \
        list_remove(threads, thread);                          \
        list_remove(thread->process->threads, (void *)thread); \
        free(thread->stack);                                   \
        free(thread);                                          \
    }

#define PROCESS_FREE(process)                \
    {                                        \
        if (process->pdir != memory_kpdir()) \
        {                                    \
            memory_free_pdir(process->pdir); \
        }                                    \
                                             \
        list_free(process->threads);         \
        free(process);                       \
    }

thread_t *alloc_thread(thread_entry_t entry, int flags)
{
    thread_t *thread = MALLOC(thread_t);
    memset(thread, 0, sizeof(thread_t));

    thread->id = TID++;

    thread->stack = malloc(STACK_SIZE);
    memset(thread->stack, 0, STACK_SIZE);

    thread->entry = entry;

    thread->esp = ((uint)(thread->stack) + STACK_SIZE);
    thread->esp -= sizeof(context_t);

    context_t *context = (context_t *)thread->esp;

    context->eflags = 0x202;
    context->eip = (u32)entry;
    context->ebp = ((uint)thread->stack + STACK_SIZE);

    if (flags & TASK_USER)
    {
        // context->cs = 0x18;
        // context->ds = 0x20;
        // context->es = 0x20;
        // context->fs = 0x20;
        // context->gs = 0x20;

        context->cs = 0x08;
        context->ds = 0x10;
        context->es = 0x10;
        context->fs = 0x10;
        context->gs = 0x10;
    }
    else
    {
        context->cs = 0x08;
        context->ds = 0x10;
        context->es = 0x10;
        context->fs = 0x10;
        context->gs = 0x10;
    }

    log("Thread with ID=%d allocated. (STACK=0x%x, ESP=0x%x)", thread->id, thread->stack, thread->esp);

    return thread;
}

process_t *alloc_process(const char *name, int flags)
{
    process_t *process = MALLOC(process_t);

    process->id = PID++;

    strncpy(process->name, name, PROCNAME_SIZE);
    process->flags = flags;
    process->threads = list_alloc();

    if (flags & TASK_USER)
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

void kill_thread(thread_t *thread)
{
    list_remove(thread->process->threads, thread);

    if (thread->process->threads->count == 0)
    {
        PROCESS_FREE(thread->process);
    }

    THREAD_FREE(thread);
}

void kill_process(process_t *process)
{
    FOREACH(i, process->threads){
        THREAD_FREE(((thread_t *)i->value))}

    PROCESS_FREE(process);
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

thread_t *running = NULL;
list_t *waiting;

esp_t shedule(esp_t esp, context_t *context);

void timer_set_frequency(int hz)
{
    u32 divisor = 119318 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    log("Timer frequency is %dhz.", hz);
}

// define in cpu/boot.s
extern u32 __stack_bottom;

void tasking_setup()
{
    running = NULL;

    waiting = list_alloc();
    threads = list_alloc();
    processes = list_alloc();

    kernel_process = process_create("kernel", 0);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);

    // Set the correct stack for the kernel main stack
    thread_t *kthread = thread_get(kernel_thread);
    free(kthread->stack);
    kthread->stack = &__stack_bottom;
    kthread->esp = ((uint)(kthread->stack) + STACK_SIZE);

    timer_set_frequency(100);
    irq_register(0, (irq_handler_t)&shedule);
}

/* --- Thread managment ----------------------------------------------------- */

void thread_yield()
{
    asm("int $32");   
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
// Look like gcc like to break this functions XD 
void thread_hold()
{
    while (running->state != THREAD_RUNNING)
        hlt();
}

#pragma GCC pop_options

THREAD thread_self()
{
    if (running == NULL)
        return -1;

    return running->id;
}

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, int flags)
{
    UNUSED(arg);

    atomic_begin();

    process_t *process = process_get(p);
    thread_t *thread = alloc_thread(entry, process->flags | flags);

    list_pushback(process->threads, thread);
    list_pushback(threads, thread);
    thread->process = process;

    if (running != NULL)
    {
        list_pushback(waiting, thread);
    }
    else
    {
        running = thread;
    }

    thread->state = THREAD_RUNNING;

    log("Thread with ID=%d child of process '%s' (ID=%d) is running.", thread->id, process->name, process->id);

    atomic_end();

    return thread->id;
}

void thread_sleep(int time)
{
    atomic_begin();
    running->state = THREAD_SLEEP;
    running->sleepinfo.wakeuptick = ticks + time;
    atomic_end();

    thread_hold();
}

void thread_wakeup(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_get(t);

    if (thread != NULL && thread->state == THREAD_SLEEP)
    {
        thread->state = THREAD_RUNNING;
        thread->sleepinfo.wakeuptick = 0;
    }

    atomic_end();
}

void *thread_wait(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_get(t);

    running->waitinfo.outcode = 0;

    if (thread != NULL)
    {
        running->waitinfo.handle = t;
        running->state = THREAD_WAIT_THREAD;
    }

    atomic_end();

    thread_hold();

    return (void *)running->waitinfo.outcode;
}

int thread_waitproc(PROCESS p)
{
    atomic_begin();

    process_t *process = process_get(p);

    running->waitinfo.outcode = 0;

    if (process != NULL)
    {
        running->waitinfo.handle = p;
        running->state = THREAD_WAIT_PROCESS;
    }

    atomic_end();

    thread_hold();

    return running->waitinfo.outcode;
}

int thread_cancel(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_get(t);

    if (thread != NULL)
    {
        thread->state = THREAD_CANCELING;
        thread->exit_value = NULL;
        log("Thread n°%d got canceled.", t);
    }

    atomic_end();

    return thread == NULL; // return 1 if canceling the thread failled!
}

void thread_exit(void *retval)
{
    atomic_begin();

    running->state = THREAD_CANCELING;
    running->exit_value = retval;

    log("Thread n°%d exited with value 0x%x.", running->id, retval);

    atomic_end();

    while (1)
        hlt();
}

void thread_dump_all()
{
    atomic_begin();

    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        thread_dump(((thread_t *)i->value)->id);
    }

    atomic_end();
}

void thread_dump(THREAD t)
{
    atomic_begin();

    thread_t *thread = thread_get(t);

    printf("\n\tThread ID=%d child of process '%s' ID=%d.", t, thread->process->name, thread->process->id);
    printf("(ESP=0x%x STACK=%x STATE=%x)", thread->esp, thread->stack, thread->state);

    atomic_end();
}

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self()
{
    if (running == NULL)
        return -1;

    return running->process->id;
}

PROCESS process_create(const char *name, int flags)
{
    atomic_begin();

    process_t *process = alloc_process(name, flags);
    list_pushback(processes, process);

    atomic_end();

    log("Process '%s' with ID=%d and PDIR=%x is running.", process->name, process->id, process->pdir);

    return process->id;
}

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

    if (dest >= 0x100000)
    {
        atomic_begin();

        // To avoid pagefault we need to switch page directorie.
        page_directorie_t *pdir = running->process->pdir;

        paging_load_directorie(process->pdir);
        process_map(process->id, dest, PAGE_ALIGN(destsz) / PAGE_SIZE);
        memset((void *)dest, 0, destsz);
        memcpy((void *)dest, (void *)src, srcsz);

        paging_load_directorie(pdir);

        atomic_end();
    }
    else
    {
        log("Elf segment ignored, not in user memory!");
    }
}

PROCESS process_exec(const char *path, const char **arg)
{
    UNUSED(arg);

    file_t *fp = file_open(NULL, path);

    if (!fp)
    {
        log("EXEC: %s file not found, exec failed!", path);
        return 0;
    }

    PROCESS p = process_create(path, TASK_USER);

    void *buffer = file_read_all(fp);
    file_close(fp);

    ELF_header_t *elf = (ELF_header_t *)buffer;

    ELF_program_t program;

    for (int i = 0; ELF_read_program(elf, &program, i); i++)
    {
        load_elfseg(process_get(p), (uint)(buffer) + program.offset, program.filesz, program.vaddr, program.memsz);
    }

    thread_create(p, (thread_entry_t)elf->entry, NULL, 0);

    free(buffer);

    return p;
}

void cancel_childs(process_t *process)
{
    FOREACH(i, process->threads)
    {
        thread_t *thread = (thread_t *)i->value;
        thread_cancel(thread->id);
    }
}

void process_cancel(PROCESS p)
{
    atomic_begin();

    if (p != kernel_process)
    {
        process_t *process = process_get(p);
        process->state = PROCESS_CANCELING;
        process->exit_code = -1;
        log("Process '%s' ID=%d canceled!", process->name, process->id);

        cancel_childs(process);
    }
    else
    {
        process_t *process = process_get(process_self());
        log("Warning! Process '%s' ID=%d tried to commit murder on the kernel!", process->name, process->id);
    }

    atomic_end();
}

void process_exit(int code)
{
    atomic_begin();

    PROCESS p = process_self();
    process_t *process = process_get(p);

    if (p != kernel_process)
    {
        process->state = PROCESS_CANCELING;
        process->exit_code = code;
        log("Process '%s' ID=%d exited with code %d.", process->name, process->id, code);

        cancel_childs(process);
        
        atomic_end();
        while (1) hlt();
    }
    else
    {
        log("Warning! Kernel try to commit suicide!");
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

uint process_alloc(uint count)
{
    return memory_alloc(running->process->pdir, count, 1);
}

void process_free(uint addr, uint count)
{
    return memory_free(running->process->pdir, addr, count, 1);
}

/* --- Sheduler ------------------------------------------------------------- */

thread_t *get_next_task()
{
    thread_t *thread = NULL;

    do
    {
        list_pop(waiting, (void *)&thread);

        switch (thread->state)
        {
            case THREAD_CANCELING:
            {
                log("Thread %d canceled!", thread->id);
                thread->state = THREAD_CANCELED;
                // TODO: cleanup the thread.
                // TODO: cleanup the process if no thread is still running.
                break;
            }
            case THREAD_SLEEP:
            {
                // Wakeup the thread
                if (thread->sleepinfo.wakeuptick >= ticks)
                {
                    thread->state = THREAD_RUNNING;
                    log("Thread %d wake up!", thread->id);
                }
                break;
            }
            case THREAD_WAIT_PROCESS:
            {
                process_t *wproc = process_get(thread->waitinfo.handle);

                if (wproc->state == PROCESS_CANCELED || wproc->state == PROCESS_CANCELING)
                {
                    thread->state = THREAD_RUNNING;
                    thread->waitinfo.outcode = wproc->exit_code;
                    log("Thread %d finish waiting process %d.", thread->id, wproc->id);
                }

                break;
            }
            case THREAD_WAIT_THREAD:
            {
                thread_t *wthread = thread_get(thread->waitinfo.handle);

                if (wthread->state == THREAD_CANCELED || wthread->state == THREAD_CANCELING)
                {
                    thread->state = THREAD_RUNNING;
                    thread->waitinfo.outcode = (uint)wthread->exit_value;
                    log("Thread %d finish waiting thread %d.", thread->id, wthread->id);
                }

                break;
            }
            default:
                break;
        }

        if (thread != NULL && thread->state != THREAD_RUNNING)
        {
            list_pushback(waiting, thread);
        }

    } while (thread == NULL || thread->state != THREAD_RUNNING);


    return thread;
}

esp_t shedule(esp_t esp, context_t *context)
{
    UNUSED(context);

    ticks++;

    if (waiting->count == 0)
        return esp;

    // Save the old context
    running->esp = esp;
    list_pushback(waiting, running);

    // Load the new context
    running = get_next_task();
    set_kernel_stack((uint)running->stack + STACK_SIZE);

    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    return running->esp;
}
