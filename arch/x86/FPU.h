#pragma once
#include "kernel/tasking.h"

void fpu_initialize(void);

void fpu_save_context(Task *task);

void fpu_load_context(Task *task);
