#pragma once

#include "system/tasking/Task.h"

#define SCHEDULER_RECORD_COUNT 1000

void scheduler_initialize();

void scheduler_did_create_idle_task(Task *task);

void scheduler_did_create_running_task(Task *task);

void scheduler_did_change_task_state(Task *task, TaskState oldstate, TaskState newstate);

bool scheduler_is_context_switch();

int scheduler_get_usage(int task_id);

Task *scheduler_running();

int scheduler_running_id();

void scheduler_yield();

uintptr_t schedule(uintptr_t current_stack_pointer);
