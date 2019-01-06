/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* tasking.c: Sheduling, messaging, sharedmemory and sheduling                */

/*
 * TODO:
 * - The name of somme function need refactor.
 * - Isolate user space process in ring 3
 * - Add a process/thread garbage colector
 * - Move the sheduler in his own file.
 * - Allow to pass parameters to thread and then return values
 * - Add priority to the round robine sheduler
 * 
 * BUG:
 * - Deadlock when using thread_sleep() when a single thread is running. 
 *   (kinda fixed by adding a dummy hidle thread)
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <skift/elf.h>
#include <skift/atomic.h>
#include <skift/logger.h>

#include "kernel/processor.h"
#include "kernel/cpu/gdt.h"
#include "kernel/cpu/irq.h"
#include "kernel/filesystem.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/system.h"

#include "kernel/tasking.h"
#include "kernel/messaging.h"
#include "kernel/shared_memory.h"

int PID = 1;
int TID = 1;

uint ticks = 0;
list_t *threads;
list_t *processes;
list_t *shared_memories;

thread_t *alloc_thread(thread_entry_t entry, int flags)
{
    thread_t *thread = MALLOC(thread_t);
    memset(thread, 0, sizeof(thread_t));

    thread->id = TID++;

    thread->stack = malloc(STACK_SIZE);
    memset(thread->stack, 0, STACK_SIZE);

    thread->entry = entry;

    thread->esp = ((uint)(thread->stack) + STACK_SIZE);
    thread->esp -= sizeof(processor_context_t);

    processor_context_t *context = (processor_context_t *)thread->esp;

    context->eflags = 0x202;
    context->eip = (u32)entry;
    context->ebp = ((uint)thread->stack + STACK_SIZE);

    if (flags & TASK_USER)
    {
        // TODO: userspace thread
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

    sk_log(LOG_FINE, "Thread with ID=%d allocated. (STACK=0x%x, ESP=0x%x)", thread->id, thread->stack, thread->esp);

    return thread;
}

void cleanup_thread(thread_t *thread)
{
    // Close all reference to/from this thread.

    // Free the stack.
    free(thread->stack);

    UNUSED(thread);
}

process_t *alloc_process(const char *name, int flags)
{
    process_t *process = MALLOC(process_t);

    process->id = PID++;

    strncpy(process->name, name, PROCNAME_SIZE);
    process->flags = flags;
    process->threads = list();
    process->inbox = list();
    process->shared = list();

    if (flags & TASK_USER)
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

void cleanup_process(process_t *process)
{
    // Close all reference to/from this process.

    // Cleanup the inbox.

    // Free all shared memory region.

    // Free all allocated memory.

    // Mark this process a dead to be free later by the garbage collector.

    UNUSED(process);
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
THREAD kernel_idle;

thread_t *running = NULL;
list_t *waiting;

esp_t shedule(esp_t esp, processor_context_t *context);

void timer_set_frequency(int hz)
{
    u32 divisor = 119318 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    sk_log(LOG_DEBUG, "Timer frequency is %dhz.", hz);
}

// define in cpu/boot.s
extern u32 __stack_bottom;

void idle()
{
    while (1)
    {
        hlt();
    }
}

void tasking_setup()
{
    running = NULL;

    waiting = list();
    threads = list();
    processes = list();
    shared_memories = list();

    kernel_process = process_create("kernel", 0);
    kernel_thread = thread_create(kernel_process, NULL, NULL, 0);
    kernel_idle = thread_create(kernel_process, idle, NULL, 0);

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
#pragma GCC optimize("O0") // Look like gcc like to break this functions XD

void thread_hold()
{
    while (running->state != THREAD_RUNNING)
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

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, int flags)
{
    UNUSED(arg);

    sk_atomic_begin();

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

    sk_log(LOG_FINE, "Thread with ID=%d ENTRY=%x child of process '%s' (ID=%d) is running.", thread->id, entry, process->name, process->id);

    sk_atomic_end();

    return thread->id;
}

void thread_sleep(int time)
{
    ATOMIC({
        running->state = THREAD_SLEEP;
        running->sleepinfo.wakeuptick = ticks + time;
    });

    thread_hold();
}

void thread_wakeup(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    if (thread != NULL && thread->state == THREAD_SLEEP)
    {
        thread->state = THREAD_RUNNING;
        thread->sleepinfo.wakeuptick = 0;
    }

    sk_atomic_end();
}

int thread_wait_thread(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    running->waitinfo.outcode = 0;

    if (thread != NULL)
    {
        running->waitinfo.handle = t;
        running->state = THREAD_WAIT_THREAD;
    }

    sk_atomic_end();

    thread_hold();

    return running->waitinfo.outcode;
}

int thread_wait_process(PROCESS p)
{
    sk_atomic_begin();

    process_t *process = process_get(p);

    running->waitinfo.outcode = 0;

    if (process != NULL)
    {
        running->waitinfo.handle = p;
        running->state = THREAD_WAIT_PROCESS;
    }

    sk_atomic_end();

    thread_hold();

    return running->waitinfo.outcode;
}

int thread_wait_message(message_t* msg)
{
    message_t *incoming;

    ATOMIC({
        thread_running()->state = THREAD_WAIT_MESSAGE;
    });
    
    thread_hold(); // Wait for the sheduler to give us a message.

    incoming = thread_running()->messageinfo.message;

    if (incoming != NULL)
    {
        memcpy(msg, incoming, sizeof(message_t));
        return 1;
    }

    return 0;
}

int thread_cancel(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    if (thread != NULL)
    {
        thread->state = THREAD_CANCELING;
        thread->exit_value = NULL;
        sk_log(LOG_DEBUG, "Thread(%d) got canceled.", t);
    }

    sk_atomic_end();

    return thread == NULL; // return 1 if canceling the thread failled!
}

void thread_exit(void *retval)
{
    sk_atomic_begin();

    running->state = THREAD_CANCELING;
    running->exit_value = retval;

    sk_log(LOG_DEBUG, "Thread(%d) exited with value 0x%x.", running->id, retval);

    sk_atomic_end();

    while (1)
        hlt();
}

void thread_dump_all()
{
    sk_atomic_begin();

    printf("\n\tCurrent thread:");
    thread_dump(running->id);
    
    printf("\n");

    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        if ((thread_t *)i->value != running)
        {
            thread_dump(((thread_t *)i->value)->id);
        }
    }

    sk_atomic_end();
}

static char* THREAD_STATES[] =
{
    "RUNNING",
    "SLEEP",
    "WAIT_THREAD",
    "WAIT_PROCESS",
    "WAIT_MESSAGE",
    "CANCELING",
    "CANCELED",
};

void thread_dump(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    printf("\n\t- ID=%d PROC=('%s', %d) ", t, thread->process->name, thread->process->id);
    printf("ESP=0x%x STACK=%x STATE=%s", thread->esp, thread->stack, THREAD_STATES[thread->state]);

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

PROCESS process_create(const char *name, int flags)
{
    process_t *process;

    ATOMIC({
        process = alloc_process(name, flags);
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

PROCESS process_exec(const char *path, const char **arg)
{
    UNUSED(arg);

    file_t *fp = file_open(NULL, path);

    if (!fp)
    {
        sk_log(LOG_WARNING, "EXEC: %s file not found, exec failed!", path);
        return 0;
    }

    PROCESS p = process_create(path, TASK_USER);

    void *buffer = file_read_all(fp);
    file_close(fp);

    elf_header_t *elf = (elf_header_t *)buffer;

    elf_program_t program;

    for (int i = 0; elf_read_program(elf, &program, i); i++)
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
    sk_atomic_begin();

    if (p != kernel_process)
    {
        process_t *process = process_get(p);
        process->state = PROCESS_CANCELING;
        process->exit_code = -1;
        sk_log(LOG_DEBUG, "Process '%s' ID=%d canceled!", process->name, process->id);

        cancel_childs(process);
    }
    else
    {
        process_t *process = process_get(process_self());
        sk_log(LOG_WARNING, "Process '%s' ID=%d tried to commit murder on the kernel!", process->name, process->id);
    }

    sk_atomic_end();
}

void process_exit(int code)
{
    sk_atomic_begin();

    PROCESS p = process_self();
    process_t *process = process_get(p);

    if (p != kernel_process)
    {
        process->state = PROCESS_CANCELING;
        process->exit_code = code;
        sk_log(LOG_DEBUG, "Process '%s' ID=%d exited with code %d.", process->name, process->id, code);

        cancel_childs(process);

        sk_atomic_end();
        while (1)
            hlt();
    }
    else
    {
        sk_log(LOG_WARNING, "Kernel try to commit suicide!");
    }

    sk_atomic_end();
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

/* --- Shared Memory -------------------------------------------------------- */

shared_memory_t *shared_memory(uint size)
{
    shared_memory_t *shm = MALLOC(shared_memory_t);

    shm->memory = (void *)memory_alloc(memory_kpdir(), size, 0);
    shm->size = size;
    shm->refcount = 0;

    sk_log(LOG_DEBUG, "Shared memory region created @%x.", shm->memory);

    list_pushback(shared_memories, shm);

    return shm;
}

void shared_memory_delete(shared_memory_t *shm)
{
    list_remove(shared_memories, shm);
    memory_free(memory_kpdir(), (uint)shm->memory, shm->size, 0);

    free(shm);

    sk_log(LOG_DEBUG, "Shared memory region deleted @%x.", shm->memory);
}

shared_memory_t *shared_memory_get(void *mem)
{
    FOREACH(i, shared_memories)
    {
        shared_memory_t *shm = (shared_memory_t *)i->value;

        if (shm->memory == mem)
        {
            return shm;
        }
    }

    return NULL;
}

void *shared_memory_create(uint size)
{
    sk_atomic_begin();

    shared_memory_t *shm = shared_memory(size);

    sk_log(LOG_DEBUG, "Shared memory region created @%x by process '%s'@%d.", shm->memory, running->process->name, running->id);

    if (shm != NULL)
    {
        shared_memory_aquire(shm->memory);
    }

    sk_atomic_end();

    return shm != NULL ? shm->memory : NULL;
}

void *shared_memory_aquire(void *mem)
{
    sk_atomic_begin();

    shared_memory_t *shm = shared_memory_get(mem);

    if (shm != NULL)
    {
        sk_log(LOG_DEBUG, "Shared memory region @%x aquire by process '%s'@%d.", shm->memory, running->process->name, running->id);
        list_pushback(running->process->shared, shm);
        shm->refcount++;

        sk_atomic_end();

        return mem;
    }
    else
    {
        sk_log(LOG_WARNING, "Process '%s'@%d tried to aquire a shared memory region @%x.", running->process->name, running->id, mem);
        sk_atomic_end();

        return NULL;
    }
}

void shared_memory_realease(void *mem)
{
    sk_atomic_begin();

    shared_memory_t *shm = shared_memory_get(mem);

    if (shm != NULL && list_containe(running->process->shared, shm))
    {
        sk_log(LOG_DEBUG, "Shared memory region @%x realease by process '%s'@%d.", shm->memory, running->process->name, running->id);
        list_remove(running->process->shared, shm);
        shm->refcount--;

        if (shm->refcount == 0)
        {
            shared_memory_delete(shm);
        }
    }
    else
    {
        sk_log(LOG_WARNING, "Process '%s'@%d tried to realease a shared memory region @%x.", running->process->name, running->id, mem);
    }

    sk_atomic_end();
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
            sk_log(LOG_DEBUG, "Thread %d canceled!", thread->id);
            thread->state = THREAD_CANCELED;
            // TODO: cleanup the thread.
            // TODO: cleanup the process if no thread is still running.

            thread = NULL;
            break;
        }
        case THREAD_SLEEP:
        {
            // Wakeup the thread
            if (thread->sleepinfo.wakeuptick <= ticks)
            {
                thread->state = THREAD_RUNNING;
                sk_log(LOG_DEBUG, "Thread %d wake up!", thread->id);
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
                sk_log(LOG_DEBUG, "Thread %d finish waiting process %d.", thread->id, wproc->id);
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
            }
            break;
        }
        case THREAD_WAIT_MESSAGE:
        {
            message_t* incoming = messaging_receive_internal(thread);

            if (incoming != NULL)
            {
                thread->state = THREAD_RUNNING;
            }
            break;
        }
        default:
            break;
        }

        if (thread != NULL && thread->state != THREAD_RUNNING)
        {
            // The thread is not a running thread, pushing it back...
            list_pushback(waiting, thread);
        }

    } while (thread == NULL || thread->state != THREAD_RUNNING);

    return thread;
}

bool is_context_switch = false;

esp_t shedule(esp_t esp, processor_context_t *context)
{
    is_context_switch = true;
    UNUSED(context);

    ticks++;

    if (waiting->count == 0)
        return esp;

    // Save the old context
    running->esp = esp;

    list_pushback(waiting, running);

    // Load the new context
    running = get_next_task();

    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    is_context_switch = false;

    return running->esp;
}
