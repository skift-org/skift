#include <skift/atomic.h>

#include "kernel/thread.h"
#include "kernel/tasking.h"
#include "kernel/garbage_colector.h"

void collect_and_free_thread(void)
{
    list_t *thread_to_free = list();

    // Search for thread with a canceled parent process.
    FOREACH(i, thread_bystate(THREADSTATE_CANCELED))
    {
        thread_t *thread = i->value;

        if (thread->process->state == PROCESS_CANCELED)
        {
            list_pushback(thread_to_free, thread);
        }
    }

    // Cleanup all of those dead threads.
    FOREACH(i, thread_to_free)
    {
        thread_t *thread = i->value;
        int thread_id = thread->id;

        sk_log(LOG_DEBUG, "free'ing thread %d", thread_id);

        thread_delete(thread);

        sk_log(LOG_DEBUG, "Thread %d free'd.", thread_id);
    }

    list_delete(thread_to_free);
}

void collect_and_free_process(void)
{
    // Ho god, this is going to be hard :/
    list_t *process_to_free = list();

    list_delete(process_to_free);
}

void garbage_colector()
{
    while (true)
    {
        thread_sleep(100); // We don't do this really often.

        // sk_log(LOG_DEBUG, "Garbage collect begin %d !", ticks);

        sk_atomic_begin();
        collect_and_free_thread();
        collect_and_free_process();
        sk_atomic_end();

        // sk_log(LOG_DEBUG, "Garbage collect end!");
    }
}

void garbage_colector_setup(void)
{

}