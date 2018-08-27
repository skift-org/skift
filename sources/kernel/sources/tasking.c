#include <stdlib.h>
#include <string.h>
#include "ds/list.h"

#include "cpu/cpu.h"
#include "cpu/gdt.h"
#include "cpu/irq.h"
#include "kernel/logging.h"
#include "kernel/tasking.h"
#include "sync/atomic.h"

esp_t task_shedule(esp_t esp, context_t *context);

extern uint ticks;

int TID = 0;
thread_t *running;

list_t *waiting;
list_t *dead;

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

    debug("Thread create with ID=%d, EIP=%x, ESP=%x!", thread->id, context->eip, thread->esp);
    dump_context(context);

    return thread;
}

void thread_free(thread_t *thread)
{
    free(thread->stack);
    free(thread);
}

/* --- Public Functions ----------------------------------------------------- */

void tasking_setup()
{
    // clear the task table.
    waiting = list_alloc();
    dead = list_alloc();

    // Create the kernel task.
    thread_create(NULL);

    irq_register(0, (irq_handler_t)&task_shedule);
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
    if (thread == NULL)
        return 0;

    debug("Thread %d cancel!", thread->id);

    atomic_begin();

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

    asm("int $32");

    while (1)
    {
        debug("Alive");
    };
}

thread_t *thread_self()
{
    return running;
}

esp_t task_shedule(esp_t esp, context_t *context)
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
    }

    list_pop(waiting, (int *)&running);
    set_kernel_stack((uint)running->stack + STACK_SIZE);

    // Load the new context
    return running->esp;
}
