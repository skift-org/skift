/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <skift/elf.h>
#include <skift/atomic.h>

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
int MID = 1;

uint ticks = 0;
list_t *threads;
list_t *processes;
list_t *channels;

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

    log("Thread with ID=%d allocated. (STACK=0x%x, ESP=0x%x)", thread->id, thread->stack, thread->esp);

    return thread;
}

void cleanup_thread(thread_t *thread)
{
    // Close all reference to/from this thread.
    UNUSED(thread);
}

process_t *alloc_process(const char *name, int flags)
{
    process_t *process = MALLOC(process_t);

    process->id = PID++;

    strncpy(process->name, name, PROCNAME_SIZE);
    process->flags = flags;
    process->threads = list_alloc();
    process->inbox = list_alloc();

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

void cleanup_process(process_t *process)
{
    // Close all reference to/from this process.
    UNUSED(process);
}

channel_t *alloc_channel(const char *name)
{
    channel_t *channel = MALLOC(channel_t);

    channel->subscribers = list_alloc();
    strncpy(channel->name, name, CHANNAME_SIZE);

    return channel;
}

message_t *alloc_message(int id, const char *name, void *payload, uint size, uint flags)
{
    message_t *message = MALLOC(message_t);

    if (payload != NULL && size > 0)
    {
        message->size = min(MSGPAYLOAD_SIZE, size);
        message->payload = malloc(message->size);
        memcpy(message->payload, payload, size);
    }
    else
    {
        message->size = 0;
        message->payload = NULL;
    }

    message->id = id;
    message->flags = flags;
    strncpy(message->name, name, CHANNAME_SIZE);

    return message;
}

void free_message(message_t *msg)
{
    free(msg->payload);
    free(msg);
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

channel_t *channel_get(const char *channel)
{
    FOREACH(i, channels)
    {
        channel_t *c = (channel_t *)i->value;

        if (strcmp(channel, c->name) == 0)
            return c;
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
    channels = list_alloc();

    kernel_process = process_create("maker.skift.kernel", 0);
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
#pragma GCC optimize("O0") // Look like gcc like to break this functions XD

void thread_hold()
{
    thread_yield(); // Let's try to save some clock cicle

    while (running->state != THREAD_RUNNING)
    {
        hlt();
    }
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

    log("Thread with ID=%d ENTRY=%x child of process '%s' (ID=%d) is running.", thread->id, entry, process->name, process->id);

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

void *thread_wait(THREAD t)
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

    return (void *)running->waitinfo.outcode;
}

int thread_waitproc(PROCESS p)
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

int thread_cancel(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    if (thread != NULL)
    {
        thread->state = THREAD_CANCELING;
        thread->exit_value = NULL;
        log("Thread n°%d got canceled.", t);
    }

    sk_atomic_end();

    return thread == NULL; // return 1 if canceling the thread failled!
}

void thread_exit(void *retval)
{
    sk_atomic_begin();

    running->state = THREAD_CANCELING;
    running->exit_value = retval;

    log("Thread n°%d exited with value 0x%x.", running->id, retval);

    sk_atomic_end();

    while (1)
        hlt();
}

void thread_dump_all()
{
    sk_atomic_begin();

    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        thread_dump(((thread_t *)i->value)->id);
    }

    sk_atomic_end();
}

void thread_dump(THREAD t)
{
    sk_atomic_begin();

    thread_t *thread = thread_get(t);

    printf("\n\tThread ID=%d child of process '%s' ID=%d.", t, thread->process->name, thread->process->id);
    printf("(ESP=0x%x STACK=%x STATE=%x)", thread->esp, thread->stack, thread->state);

    sk_atomic_end();
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
    process_t *process;

    ATOMIC({
        process = alloc_process(name, flags);
        list_pushback(processes, process);
    });

    log("Process '%s' with ID=%d and PDIR=%x is running.", process->name, process->id, process->pdir);

    return process->id;
}

void load_elfseg(process_t *process, uint src, uint srcsz, uint dest, uint destsz)
{
    log("Loading ELF segment: SRC=0x%x(%d) DEST=0x%x(%d)", src, srcsz, dest, destsz);

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
        log("Process '%s' ID=%d canceled!", process->name, process->id);

        cancel_childs(process);
    }
    else
    {
        process_t *process = process_get(process_self());
        log("Warning! Process '%s' ID=%d tried to commit murder on the kernel!", process->name, process->id);
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
        log("Process '%s' ID=%d exited with code %d.", process->name, process->id, code);

        cancel_childs(process);

        sk_atomic_end();
        while (1)
            hlt();
    }
    else
    {
        log("Warning! Kernel try to commit suicide!");
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

/* --- Messaging ------------------------------------------------------------ */

uint messaging_id()
{
    uint id;

    ATOMIC({
        id = MID++;
    });

    return id;
}

int messaging_send_internal(PROCESS from, PROCESS to, int id, const char *name, void *payload, uint size, uint flags)
{
    if (from == to)
    {
        log("PROCESS=%d try to send a message to himself!");
        return 0;
    }

    // log("Sending message ID=%d from %d to %d.", id, from, to);

    process_t *process = process_get(to);

    if (process == NULL)
    {
        return 0;
    }

    if (process->inbox->count > 1024)
    {
        log("PROCESS=%d inbox is full!", to);
        return 0;
    }

    message_t *message = alloc_message(id, name, payload, size, flags);

    message->from = process_self();
    message->to = to;

    list_pushback(process->inbox, (void *)message);

    return id;
}

int messaging_send(PROCESS to, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    ATOMIC({
        id = messaging_send_internal(process_self(), to, messaging_id(), name, payload, size, flags);
    });

    return id;
}

// TODO: broadcasting
int messaging_broadcast(const char *channel, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    sk_atomic_begin();

    channel_t *c = channel_get(channel);

    if (c != NULL)
    {
        id = messaging_id();

        FOREACH(p, c->subscribers)
        {
            messaging_send_internal(process_self(), ((process_t *)p->value)->id, id, name, payload, size, flags);
        }
    }

    sk_atomic_end();

    return id;
}

int messaging_receive(message_t *msg)
{
    ATOMIC({
        running->state = THREAD_WAIT_MESSAGE;
    });

    thread_hold();

    message_t *incoming = running->messageinfo.message;

    if (incoming != NULL)
    {
        memcpy(msg, incoming, sizeof(message_t));
        return 0;
    }

    return 1;
}

int messaging_payload(void *buffer, uint size)
{
    message_t *incoming = running->messageinfo.message;

    if (incoming != NULL && incoming->size > 0 && incoming->payload != NULL)
    {
        memcpy(buffer, incoming->payload, min(size, incoming->size));
        return 0;
    }

    return 1;
}

int messaging_subscribe(const char *channel)
{
    sk_atomic_begin();
    {
        channel_t *c = channel_get(channel);

        if (c == NULL)
        {
            c = alloc_channel(channel);
            list_pushback(channels, c);
        }

        list_pushback(c->subscribers, running->process);
    }
    sk_atomic_end();

    return 0;
}

int messaging_unsubscribe(const char *channel)
{
    sk_atomic_begin();
    {
        channel_t *c = channel_get(channel);

        if (c != NULL)
        {
            list_remove(c->subscribers, running->process);
        }
    }
    sk_atomic_end();

    return 0;
}

/* --- Shared Memory -------------------------------------------------------- */

//// SHARED_MEMORY sharedmem_create(uint size)
//// {
////
//// }
////
//// void sharedmem_destroy(SHARED_MEMORY shm)
//// {
////
//// }
////
//// uint sharedmem_mount(SHARED_MEMORY shm)
//// {
////
//// }
////
//// void sharedmem_unmount(SHARED_MEMORY shm)
//// {
////
//// }

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

                cleanup_thread(thread);
                thread = NULL;
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
            case THREAD_WAIT_MESSAGE:
            {
                if (thread->process->inbox->count > 0)
                {
                    thread->state = THREAD_RUNNING;

                    if (thread->messageinfo.message != NULL)
                    {
                        free_message(thread->messageinfo.message);
                    }

                    message_t *message;
                    list_pop(thread->process->inbox, (void **)&message);
                    thread->messageinfo.message = message;
                    //log("Thread %d recivied message ID=%d from %d to %d.", thread->id, message->id, message->from, message->to);
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