#pragma once
#include "types.h"
#include "kernel/tasking.h"

#define MYSELF -1 // so you can KILL(MYSELF) :3

u32 shedule(u32 esp);

pid_t start(task_t *task);

void sleep(u32 ticks);
void wait(pid_t pid);
void kill(pid_t pid);

void yield();
void yield_to(pid_t pid);
