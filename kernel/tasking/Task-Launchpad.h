#pragma once

#include <abi/Launchpad.h>

#include "kernel/tasking/Task.h"

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid);

Result task_exec(Task *parent_task, Launchpad *launchpad);