#pragma once
#include "types.h"
#include "utils.h"

#define STACK_SIZE 4096

typedef u32 esp_t;
typedef void (*thread_entry_t)();

typedef PACKED(struct)
{
    int id;
    void *stack;
    uint esp;
    thread_entry_t entry;
}
thread_t;

void tasking_setup();

thread_t *thread_create(thread_entry_t entry);
thread_t *thread_create_kernel(thread_entry_t entry);
int thread_cancel(thread_t *thread);
void thread_exit();
thread_t *thread_self();