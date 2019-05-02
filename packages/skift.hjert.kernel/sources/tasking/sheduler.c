/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>
#include <skift/logger.h>

#include "kernel/cpu/irq.h"
#include "kernel/processor.h"
#include "kernel/tasking.h"
#include "kernel/sheduler.h"

thread_t* running; 
uint ticks;

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
                // sk_log(LOG_DEBUG, "Thread %d wake up!", t->id);
            }

        } while (t->stack == THREADSTATE_RUNNING);
    }
}

bool is_context_switch = false;
reg32_t shedule(reg32_t sp, processor_context_t *context)
{
    //sk_log(LOG_DEBUG, "Current thread %d(%s) with eip@%08x.", running->id, running->process->name, context->eip);
    UNUSED(context);
    is_context_switch = true;

    // Save the old context
    running->sp = sp;

    // Update the system ticks
    ticks++;
    wakeup_sleeping_threads();

    // Get the next thread
    if (!list_peek_and_pushback(thread_bystate(THREADSTATE_RUNNING), (void **)&running))
    {
        // Or the idle thread if there are no running threads.
        running = &idle;
    }

    // Restore the context
    // TODO: set_kernel_stack(...);
    paging_load_directorie(running->process->pdir);
    paging_invalidate_tlb();

    is_context_switch = false;

    //sk_log(LOG_DEBUG, "Now current thread is %d(%s)", running->id, running->process->name);

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

void sheduler_setup(thread_t* main_kernel_thread, PROCESS kernel_process)
{
    running = main_kernel_thread;

    // Create the idle thread.
    memset(&idle, 0, sizeof(thread_t));
    idle.id = -1;
    thread_setentry(&idle, idle_code, false);
    thread_attach_to_process(&idle, process_get(kernel_process));
    thread_setready(&idle);

    timer_set_frequency(1000);
    irq_register(0, (irq_handler_t)&shedule);
}

process_t* sheduler_running_process()
{
    return running->process;
}

thread_t* sheduler_running_thread()
{
    return running;
}

int sheduler_running_thread_id()
{
    if (running != NULL)
    {
        return running->id;
    }
    else
    {
        return -1;
    }
}

void sheduler_yield()
{
    asm("int $32");
}