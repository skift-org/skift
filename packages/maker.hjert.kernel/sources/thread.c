#include <string.h>
#include <skift/logger.h>

#include "kernel/thread.h"
#include "kernel/system.h"

static thread_t  threads[MAX_THREAD];
static thread_t *running_thread;

static const char *THREAD_STATE[] = {"EMBRYO", "ALIVE", "SLEEPING", "DYING", "DEAD", "DUST"};

/* --- Public functions ----------------------------------------------------- */

void thread_setup(void)
{

    for (int i = 0; i < MAX_THREAD; i++)
    {
        threads[i].id = i;
        threads[i].state = THREAD_DUST;
    }
}

void thread_dump(void)
{
    printf("\n\tRunning threads:");

    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *thread = &threads[i];

        if (thread->state != THREAD_DUST)
        {
            printf("\n\t'%s' STATE=%s IS=%08x SP=%08x", thread->name, THREAD_STATE[thread->state], thread->ip, thread->sp);
        }
    }
}

thread_t *thread_by_state(int start, thread_state_t state)
{
    for (int i = 0; i < MAX_THREAD; i++)
    {
        thread_t *thread = &threads[(start + i) % MAX_THREAD];

        if (thread->state == state)
        {
            return thread;
        }
    }

    return NULL;
}

/* --- Thread building API -------------------------------------------------- */

thread_t *thread(const char *name, bool user)
{
    thread_t *thread = thread_by_state(0, THREAD_DEAD);

    if (thread == NULL)
    {
        sk_log(LOG_SEVERE, "We are running out of free threads!");
        return NULL;
    }

    thread->state = THREAD_EMBRYO;
    thread->user = user;

    // Copy the name
    strncpy(thread->name, name, MAX_THREAD_NAMESIZE);

    // Setup the stack
    memset(thread->stack, 0, MAX_THREAD_STACKSIZE);
    thread->sp = (uint)thread->stack + MAX_THREAD_STACKSIZE;

    return thread;
}

void thread_delete(thread_t *thread)
{
    thread->state = THREAD_DUST;
}

void thread_stackpush(thread_t *thread, void *value, int size)
{
    if (thread->state == THREAD_EMBRYO)
    {
        thread->sp -= size;
        memcpy((void*)thread->sp, value, size);
    }
    else
    {
        PANIC("Operating on a non-ambryo thread!");
    }
}

void thread_setentrypoint(thread_t *thread, void *entry)
{
    if (thread->state == THREAD_EMBRYO)
    {
        thread->ip = (reg32_t)entry;
    }
    else
    {
        PANIC("You are operating on a non-ambryo thread!");
    }
}

void thread_ready(thread_t *thread)
{
    if (thread->state == THREAD_EMBRYO)
    {
        processor_context_t context = {0};

        context.eflags = 0x202;
        context.eip = thread->ip;
        context.ebp = ((uint)thread->stack + MAX_THREAD_STACKSIZE);

        // TODO: running in ring 3
        context.cs = 0x08;
        context.ds = 0x10;
        context.es = 0x10;
        context.fs = 0x10;
        context.gs = 0x10;

        thread_stackpush(thread, &context, sizeof(processor_context_t));

        thread->state = THREAD_ALIVE;
    }
    else
    {
        PANIC("You are operating on a non-ambryo thread!");
    }
}

/* --- Thread waiting ------------------------------------------------------- */

