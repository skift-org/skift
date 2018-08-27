#pragma once
#include "types.h"




#define TASK_MAX_COUNT 16
#define TASK_NAME_SIZE 64
#define TASK_STACK_SIZE 4096

typedef u32 esp_t;
typedef void (*thread_entry_t)();


typedef PACKED(struct)
{
    int id;
    u32 esp;
    thread_entry_t entry;
    u8 stack[TASK_STACK_SIZE];
}
thread_t;

void tasking_setup();
thread_t* thread_create(thread_entry_t entry);

esp_t task_shedule(esp_t esp, context_t *context);

#define PROCESS_STACK_SIZE 4096
