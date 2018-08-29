#pragma once
#include <stdbool.h>
#include "types.h"
#include "utils.h"
#include "ds/list.h"
#include "kernel/paging.h"

#define PROCNAME_SIZE 128
#define STACK_SIZE 4096

typedef u32 esp_t;
typedef void (*thread_entry_t)();

typedef struct
{
    int id;
    char name[PROCNAME_SIZE];
    bool user;
    list_t *threads;
    page_directorie_t *pdir;
} process_t;

typedef struct
{
    int id;
    void *stack;
    uint esp;
    thread_entry_t entry;
    process_t *process;
} thread_t;

void tasking_setup();

thread_t *thread_create(thread_entry_t entry);
int thread_cancel(thread_t *thread);
void thread_exit();
thread_t *thread_self();

process_t *process_exec(const char *path, int argc, char **argv);
void process_cancel(process_t *process);
process_t *process_self();
void process_exit(int code);

int process_map(process_t *process, uint addr, uint count);
int process_unmap(process_t *process, uint addr, uint count);