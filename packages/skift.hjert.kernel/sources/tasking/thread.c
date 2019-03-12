#include <string.h>
#include <skift/atomic.h>

#include "kernel/tasking.h"
#include "kernel/system.h"
#include "kernel/thread.h"
#include "kernel/processor.h"
#include "kernel/sheduler.h"

static int TID = 1;
static list_t *threads;
static list_t *threads_bystates[THREADSTATE_COUNT];

void thread_setup(void)
{
    threads = list();

    if (threads == NULL)
    {
        PANIC("Failed to allocate thread list!");
    }

    for (int i = 0; i < THREADSTATE_COUNT; i++)
    {
        threads_bystates[i] = list();

        if (threads_bystates[i] == NULL)
        {
            PANIC("Failled to allocate threadbystate list!");
        }
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

void thread_delete(thread_t *thread)
{
    if (thread->state != THREADSTATE_NONE)
        thread_setstate(thread, THREADSTATE_NONE);

    list_remove(threads, thread);

    if (thread->process != NULL)
        list_remove(thread->process->threads, thread);

    // Now no one should still have a ptr to us we can die in peace.
    free(thread);
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

void thread_dumpall()
{
    sk_atomic_begin();

    printf("\n\tCurrent thread:");
    thread_dump(sheduler_running_thread());

    printf("\n");

    printf("\n\tThreads:");

    FOREACH(i, threads)
    {
        thread_t *t = i->value;
        if (t != sheduler_running_thread() && t->state != THREADSTATE_NONE)
            thread_dump(t);
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
    }

    sk_atomic_end();
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

list_t* thread_bystate(thread_state_t state)
{
    return threads_bystates[state];
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

void thread_setentry(thread_t *t, thread_entry_t entry, bool user)
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

void thread_attach_to_process(thread_t *t, process_t *p)
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

void thread_setready(thread_t *t)
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
}