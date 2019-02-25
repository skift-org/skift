/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>

#include "kernel/cpu/irq.h"
#include "kernel/processor.h"
#include "kernel/tasking.h"
#include "kernel/sheduler.h"

static thread_t* running;
static uint ticks;

/* --- Idle thread ---------------------------------------------------------- */
static thread_t idle;
void idle_code()
{
    while (1)
    {
        hlt();
    }
}

void wakeup_sleeping_threads(void)
{
    if (!list_empty(thread_bystate(THREADSTATE_WAIT_TIME)))
    {
        thread_t *t;

        do
        {
            list_peek(thread_bystate(THREADSTATE_WAIT_TIME), (void **)&t);

            if (t->wait.time.wakeuptick <= ticks)
            {
                thread_setstate(t, THREADSTATE_RUNNING);
                sk_log(LOG_DEBUG, "Thread %d wake up!", t->id);
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
    if (list_pop(thread_bystate(THREADSTATE_RUNNING), (void **)&running))
    {
        list_pushback(thread_bystate(THREADSTATE_RUNNING), running);
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

/* --- Public functions ----------------------------------------------------- */

void timer_set_frequency(int hz)
{
    u32 divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    sk_log(LOG_DEBUG, "Timer frequency is %dhz.", hz);
}

void sheduler_setup(thread_t* main_kernel_thread)
{
    running = main_kernel_thread;

    // Create the idle thread.
    idle.id = -1;
    thread_setentry(&idle, idle_code, false);
    thread_attach_to_process(&idle, process_get(kernel_process));
    thread_setready(&idle);

    timer_set_frequency(100);
    irq_register(0, (irq_handler_t)&shedule);
}