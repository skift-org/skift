#pragma once

#include "kernel/tasking/Task.h"

Path *task_resolve_directory(Task *task, const char *buffer);

Result task_set_directory(Task *task, const char *buffer);

Result task_get_directory(Task *task, char *buffer, uint size);
