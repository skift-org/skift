#include <stdlib.h>

#include "cpu/cpu.h"
#include "ds/sllist.h"
#include "sync/atomic.h"

#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/physical.h"
#include "kernel/process.h"

pid_t PID = 0;

process_t * current;
sllist_t * running;
sllist_t * stopped;

void process_setup()
{
    running = sll_new();
    stopped = sll_new();
}

/* --- Process running states ----------------------------------------------- */

process_t * process_new(uint entry, page_directorie_t* page_directorie, int user)
{
    process_t * proc = MALLOC(process_t);

    atomic_begin();
    proc->pid = PID++;
    atomic_end();

    proc->stack = malloc(PROC_STACKSZ);
    proc->esp = (uint)proc->stack; + PROC_STACKSZ;

    proc->esp -= sizeof(context_t);
    context_t * context = (context_t*)proc->esp;
    
    context->eflags = 0x202;
    context->cs = 0x08;
    context->eip = (u32)entry;
    context->ds = 0x10;
    context->es = 0x10;
    context->fs = 0x10;
    context->gs = 0x10;

    proc->page_directorie = page_directorie;

    debug("Process allocated! (PID=%d EIP=Ox%x ESP=Ox%x PD=0x%x)", proc->pid, context->eip, proc->esp, page_directorie);

    return proc;
}

process_t * process_exec_kernel(uint entry)
{
    process_t * proc = process_new(entry, get_kernel_page_dir(), false);
    
    atomic_begin();
    sll_add((int)proc, running);
    atomic_end();

    return proc;
}

process_t * process_exec(const char * path)
{

}

int process_kill(process_t * process)
{

}

int process_exit()
{

}

/* --- Process memory ------------------------------------------------------- */

uint process_mem_mmap(process_t* process, uint virtual, size_t count)
{
    uint page = virtual / PAGE_SIZE;
}

uint process_mem_free(process_t* process, uint virtual, size_t count)
{
    uint page = virtual / PAGE_SIZE;
}

/* --- Sheduling ------------------------------------------------------------ */