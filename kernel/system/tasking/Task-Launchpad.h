#pragma once

#include <abi/Launchpad.h>

#include "system/tasking/Task.h"

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid);

Result task_exec(Task *parent_task, Launchpad *launchpad);