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

uint ticks = 0;
list_t *processes;

/* --- thread table --------------------------------------------------------- */

thread_t idle;
void idle_code() { while (1) hlt(); }

thread_t thread_table[MAX_THREAD];

void thread_setup(void)
{
    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *t = &thread_table[i];

        t->id = i;
        t->state = THREAD_FREE;
    }
}

thread_t *thread_getbystate(int start, thread_state_t state)
{
    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *t = &thread_table[(start + i) % MAX_THREAD];
        if (t->state == state)
            return t;
    }

    return NULL;
}

thread_t *thread_getbyid(int id)
{
    if (id >= 0 && id < MAX_THREAD)
    {
        return &thread_table[id];
    }

    return NULL;
}



thread_t *thread(thread_entry_t entry, bool user)
{
    thread_t *t = thread_getbystate(0, THREAD_FREE);

    if (t == NULL)
    {
        PANIC("No more free threads!");
    }

    memset(t->stack, 0, MAX_THREAD_STACKSIZE);

    t->entry = entry;

    t->sp = (reg32_t)(&t->stack[0] + MAX_THREAD_STACKSIZE);

    t->sp -= sizeof(processor_context_t);
    processor_context_t *context = (processor_context_t *)t->sp;

    context->eflags = 0x202;
    context->eip = (u32)entry;
    context->ebp = ((uint)t->stack + MAX_THREAD_STACKSIZE);

    if (user)
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

    sk_log(LOG_FINE, "Thread with ID=%d allocated. (STACK=0x%x, SP=0x%x)", t->id, t->stack, t->sp);

    return t;
}

process_t *alloc_process(const char *name, bool user)
{
    process_t *process = MALLOC(process_t);

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
THREAD kernel_idle;

thread_t *running = NULL;

esp_t shedule(esp_t esp, processor_context_t *context);

void timer_set_frequency(int hz)
{
    u32 divisor = 119318 / hz;
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
    kernel_idle = thread_create(kernel_process, idle_code, NULL, 0);

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

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, bool user)
{
    UNUSED(arg);

    sk_atomic_begin();

    process_t *process = process_get(p);
    thread_t *t = thread(entry, process->user | user);

    list_pushback(process->threads, t);
    t->process = process;

    if (running == NULL)
    {
        running = t;
    }

    t->state = THREAD_RUNNING;

    sk_log(LOG_FINE, "Thread with ID=%d ENTRY=%x child of process '%s' (ID=%d) is running.", t->id, entry, process->name, process->id);

    sk_atomic_end();

    return t->id;
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

    thread_t *thread = thread_getbyid(t);

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

    thread_t *thread = thread_getbyid(t);

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

int thread_wait_message(message_t *msg)
{
    ATOMIC({
        thread_running()->state = THREAD_WAIT_MESSAGE;
    });

    thread_hold(); // Wait for the sheduler to give us a message.

    message_t *incoming = thread_running()->messageinfo.message;

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

    thread_t *thread = thread_getbyid(t);

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
    thread_dump(running);

    printf("\n");

    printf("\n\tThreads:");

    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *t = &thread_table[i];
        if (t != running && t->state != THREAD_FREE) thread_dump(t);
    }

    sk_atomic_end();
}

static char *THREAD_STATES[] =
    {
        "RUNNING",
        "SLEEP",
        "WAIT_THREAD",
        "WAIT_PROCESS",
        "WAIT_MESSAGE",
        "CANCELING",
        "CANCELED",
};

void thread_dump(thread_t *t)
{
    sk_atomic_begin();

    printf("\n\t- ID=%d PROC=('%s', %d) ", t->id, t->process->name, t->process->id);
    printf("ESP=%08x STACK=%08x %s", t->sp, t->stack, THREAD_STATES[t->state]);

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

PROCESS process_exec(const char *path, const char **arg)
{
    UNUSED(arg);

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

/* --- Sheduler ------------------------------------------------------------- */

void sheduler_update_threads(void)
{
    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *t = thread_getbyid(i);

        switch (t->state)
        {

        case THREAD_SLEEP:
        {
            if (t->sleepinfo.wakeuptick >= ticks)
            {
                t->state = THREAD_RUNNING;
                sk_log(LOG_DEBUG, "Thread %d wake up!", t->id);
            }
        }
        break;

        case THREAD_WAIT_THREAD:
        {
            // Get the thread we are waiting.
            thread_t *wthread = thread_getbyid(t->waitinfo.handle);

            if (wthread->state == THREAD_CANCELED ||
                wthread->state == THREAD_CANCELING)
            {
                t->state = THREAD_RUNNING;
                t->waitinfo.outcode = (uint)wthread->exit_value;
                sk_log(LOG_DEBUG, "Thread %d finish waiting thread %d.", t->id, wthread->id);
            }
        }
        break;

        case THREAD_WAIT_PROCESS:
        {
            // Get the process we are waiting.
            process_t *wproc = process_get(t->waitinfo.handle);

            if (wproc->state == PROCESS_CANCELED ||
                wproc->state == PROCESS_CANCELING)
            {
                t->state = THREAD_RUNNING;
                t->waitinfo.outcode = wproc->exit_code;
                sk_log(LOG_DEBUG, "Thread %d finish waiting process %d.", t->id, wproc->id);
            }
        }
        break;

        case THREAD_WAIT_MESSAGE:
        {
            message_t *incoming = messaging_receive_internal(t);

            if (incoming != NULL)
            {
                t->state = THREAD_RUNNING;
            }
        }
        break;

        case THREAD_CANCELING:
        {
            sk_log(LOG_DEBUG, "Thread %d canceled!", t->id);
            t->state = THREAD_CANCELED;
        }
        break;

        case THREAD_CANCELED:
        {
            // TODO: cleanup the process if the process is stopped.
        }
        break;

        default:
            break;
        }
    }
}

thread_t* sheduler_next_thread(void)
{
    thread_t* t = thread_getbystate(running->id + 1, THREAD_RUNNING);

    if (t == NULL)
    {
    }

    return t;
}

bool is_context_switch = false;
esp_t shedule(esp_t sp, processor_context_t *context)
{
    is_context_switch = true;
    UNUSED(context);

    ticks++;

    // Save the old context
    running->sp = sp;

    sheduler_update_threads();
    running = sheduler_next_thread();

    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    is_context_switch = false;

    return running->sp;
}
