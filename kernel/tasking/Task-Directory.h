#pragma once

#include <libutils/Path.h>

#include "kernel/tasking/Task.h"

Path task_resolve_directory(Task *task, Path &path);

Result task_set_directory(Task *task, Path &path);

Path task_get_directory(Task *task);
