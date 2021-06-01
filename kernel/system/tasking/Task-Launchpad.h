#pragma once

#include <abi/Launchpad.h>

#include "system/tasking/Task.h"

HjResult task_launch(Task *parent_task, Launchpad *launchpad, int *pid);

HjResult task_exec(Task *parent_task, Launchpad *launchpad);