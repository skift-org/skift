#pragma once
#include "types.h"
#include "ds/list.h"
#include "kernel/paging.h"

#define PROCNAME_SIZE 64
#define THREADSTACK_SIZE 4096

struct process;

typedef PACKED(struct)
{
    uint id;
    u32 esp;
    u8 stack[THREADSTACK_SIZE];
    
    struct process * proc
}
thread_t;

typedef struct process
{
    int id;
    char name[PROCNAME_SIZE];
    page_directorie_t * pdir;
    
    list_t * threads;
} process_t;