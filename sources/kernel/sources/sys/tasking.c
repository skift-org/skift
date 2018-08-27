#include <stdlib.h>
#include <string.h>
#include "ds/list.h"

#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "kernel/logging.h"
#include "kernel/tasking.h"
#include "sync/atomic.h"

extern uint ticks;

int TID = 0;
thread_t * running;
list_t* waiting;

/* --- Public Functions ----------------------------------------------------- */

void tasking_setup()
{
    // clear the task table.
    waiting = list_alloc();

    // Create the kernel task.
    thread_create(NULL);

    irq_register(0, (irq_handler_t)&task_shedule);
}

thread_t* thread_create(thread_entry_t entry)
{
    atomic_begin();


    thread_t * task = MALLOC(thread_t);
    memset(task, 0, sizeof(thread_t));

    task->id = TID++;
    task->entry = entry;

    // Setup the stack of the task;
    task->esp = (u32)&task->stack + TASK_STACK_SIZE;
    task->esp -= sizeof(context_t);
    context_t * context = (context_t*)task->esp;

    context->eflags = 0x202;
    context->cs = 0x08;
    context->eip = (u32)entry;
    context->ds = 0x10;
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;

    debug("Thread create with ID=%d, EIP=%x, ESP=%x!", task->id, context->eip, task->esp);

    if (running == NULL)
    {
        running = task;
    }
    else
    {
        list_pushback(waiting, (int)task);
    }

    atomic_end();

    return task;
}

esp_t task_shedule(esp_t esp, context_t *context)
{
    ticks++;

    UNUSED(context);

    // Save the old context
    running->esp = esp;

    list_pushback(waiting, (int)running);
    list_pop(waiting, (int*)&running);

    // Load the new context
    return running->esp;
}
