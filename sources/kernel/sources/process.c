#include <string.h>
#include "cpu/cpu.h"
#include "kernel/memory.h"
#include "kernel/logging.h"
#include "kernel/process.h"

int PID = 0;

process_t *current;

list_t *threadlist;
list_t *proclist;

void process_setup()
{
    threadlist = list_alloc();
    proclist = list_alloc();
}

uint shedule(uint esp)
{
}

/* --- Process -------------------------------------------------------------- */

process_t *process_alloc(const char *name, int user)
{
    process_t *process = MALLOC(process_t);

    process->id = PID++;
    strncpy(process->name, name, PROCNAME_SIZE);
    process->threads = list_alloc();

    if (user)
    {
        process->pdir = memory_construct_memory_space();
    }
    else
    {
        process->pdir = get_kernel_page_dir();
    }

    return process;
}

void process_kill(process_t *process)
{
    free(process);
}

thread_t *thread_alloc(process_t *process, uint entry)
{
    thread_t *thread = MALLOC(thread_t);

    thread->proc = process;
    
    return thread;
}

void thread_kill(thread_t *thread)
{
    free(thread);
}
