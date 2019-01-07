/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/atomic.h>
#include <skift/list.h>
#include <skift/logger.h>

#include "kernel/memory.h"
#include "kernel/tasking.h"
#include "kernel/shared_memory.h"

static list_t *shared_memories;

/* --- Constructor/Destructor ----------------------------------------------- */

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

/* --- Shared Memory -------------------------------------------------------- */

void shared_memory_setup(void)
{
    shared_memories = list();
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

    sk_log(LOG_DEBUG, "Shared memory region created @%x by process '%s'@%d.", shm->memory, thread_running()->process->name, thread_running()->id);

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
        sk_log(LOG_DEBUG, "Shared memory region @%x aquire by process '%s'@%d.", shm->memory, thread_running()->process->name, thread_running()->id);
        list_pushback(thread_running()->process->shared, shm);
        shm->refcount++;

        sk_atomic_end();

        return mem;
    }
    else
    {
        sk_log(LOG_WARNING, "Process '%s'@%d tried to aquire a shared memory region @%x.", thread_running()->process->name, thread_running()->id, mem);
        sk_atomic_end();

        return NULL;
    }
}

void shared_memory_realease(void *mem)
{
    sk_atomic_begin();

    shared_memory_t *shm = shared_memory_get(mem);

    if (shm != NULL && list_containe(thread_running()->process->shared, shm))
    {
        sk_log(LOG_DEBUG, "Shared memory region @%x realease by process '%s'@%d.", shm->memory, thread_running()->process->name, thread_running()->id);
        list_remove(thread_running()->process->shared, shm);
        shm->refcount--;

        if (shm->refcount == 0)
        {
            shared_memory_delete(shm);
        }
    }
    else
    {
        sk_log(LOG_WARNING, "Process '%s'@%d tried to realease a shared memory region @%x.", thread_running()->process->name, thread_running()->id, mem);
    }

    sk_atomic_end();
}
