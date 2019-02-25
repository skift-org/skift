#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>

#include "kernel/paging.h"
#include "kernel/protocol.h"
#include "kernel/limits.h"

/* --- Process -------------------------------------------------------------- */

#include "kernel/process.h"
#include "kernel/thread.h"

/* --- Tasking initialisation ----------------------------------------------- */

void tasking_setup();

/* --- Thread managment ----------------------------------------------------- */

THREAD thread_self(); // Return a handle to the current thread.
thread_t *thread_running();
void thread_hold();

// Create a new thread of a selected process.
THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, bool user);

bool thread_cancel(THREAD t, int exitvalue); // Cancel the selected thread.
void thread_exit(int exitvalue);             // Exit the current thread and return a value.

void thread_sleep(int time);  // Send the current thread to bed.
void thread_wakeup(THREAD t); // Wake up the slected thread

bool thread_wait_thread(THREAD t, int *exitvalue);   // Wait for the selected thread to exit and return the exit value
bool thread_wait_process(PROCESS p, int *exitvalue); // Wait for the selected process to exit and return the exit value.
void thread_yield();                                 // Yield to the next thread.

void thread_dump_all();
void thread_dump(thread_t *t);

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self(); // Return a handler to the current process.
process_t *process_running();
process_t *process_get(PROCESS process);

PROCESS process_create(const char *name, bool user); // Create a new process.

bool process_cancel(PROCESS p, int exitvalue); // Cancel the selected process.
void process_exit(int code);                   // Exit the current process and send a exit code.

int process_map(PROCESS p, uint addr, uint count);   // Map memory to the process memory space.
int process_unmap(PROCESS p, uint addr, uint count); // Unmap memory from the current thread.

uint process_alloc(uint count);           // Alloc some some memory page to the process memory space.
void process_free(uint addr, uint count); // Free perviously allocated memory.

PROCESS process_exec(const char *filename, const char **argv); // Load a ELF executable, create a adress space and run it.
