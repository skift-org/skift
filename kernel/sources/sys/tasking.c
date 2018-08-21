#include <string.h>

#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "kernel/logging.h"
#include "kernel/tasking.h"
#include "sync/atomic.h"

extern uint ticks;
pid_t current_task = -1;
task_t tasks[TASK_MAX_COUNT];
u32 running_task_count = 0;

pid_t get_task_by_state(task_state_t state)
{
        
    for(pid_t tid = 0; tid < TASK_MAX_COUNT; tid++)
    {
        task_t * task = &tasks[tid];
        if (task->state == state) return tid;
    }

    return -1;
}

pid_t get_next_task_by_state(pid_t base, task_state_t state)
{
    
    for(u32 offset = 0; offset < TASK_MAX_COUNT; offset++)
    {
        pid_t tid = (base + offset + 1) %TASK_MAX_COUNT;
        task_t * task = &tasks[tid];
        if (task->state == state) return tid;
    }
    
    return -1;
}

void stack_push(task_t* task, u32 value)
{
    task->esp -= 4;
    *((u32*)task->esp) = value;
}

/* --- Public Functions ----------------------------------------------------- */

void task_setup()
{

    // clear the task table.
    memset(&tasks, 0, sizeof(task_t) * TASK_MAX_COUNT);

    // setup entries
    for(pid_t tid = 0; tid < TASK_MAX_COUNT; tid++)
    {
        task_t * task = &tasks[tid];
        task->id = tid;
        task->state = TASK_FREE;
    }

    // Create the kernel task.
    task_start_named(NULL, "kernel");

    irq_register(0, (irq_handler_t)&task_shedule);
}

pid_t task_start_named(task_entry_t entry, string name)
{
    atomic_begin();

    pid_t free_task = get_task_by_state(TASK_FREE);

    if (free_task == -1) 
    {
        panic("Out of task table entries!");
        return -1;
    }
    
    task_t * task = &tasks[free_task];
    memset(task, 0, sizeof(task_t));

    task->id = free_task;
    task->entry = entry;
    task->state = TASK_RUNNING;

    // copy the name of the task

    strncpy((char*)&task->name, name, TASK_NAME_SIZE);
    task->name[TASK_NAME_SIZE - 1] = '\0';

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

    if (current_task == -1) current_task = free_task;

    running_task_count++;

    debug("%s is now running! (PID=%d, EIP=%x, ESP=%x)", name, task->id, context->eip, task->esp);

    atomic_end();

    return task->id;
}

esp_t task_shedule(esp_t esp) 
{
    ticks++;
    
    // Save the old context
    tasks[current_task].esp = esp;

    pid_t next_task = get_next_task_by_state(current_task, TASK_RUNNING);
    current_task = next_task;

    // Load the new context
    return tasks[current_task].esp;
}
