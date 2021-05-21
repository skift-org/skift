#pragma once

#include "system/tasking/Task.h"

void fpu_initialize();

void fpu_save_context(Task *task);

void fpu_load_context(Task *task);

void fpu_init_context(Task *task);