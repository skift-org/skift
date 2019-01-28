#pragma once

#include <skift/list.h>

struct s_process;
struct s_thread;

typedef enum 
{
    PROCESS_ALIVE,
    PROCESS_DYING,
    PROCESS_DEAD,
} process_state_t;

typedef struct s_process
{
    list_t *files;   // Opened files
    list_t *threads; // Running threads

    int exitval;
} process_t;

typedef struct s_thread
{
    process_t *process;

    int exitval;
} thread_t;

void process_create(void);
void process_destroy(void);
