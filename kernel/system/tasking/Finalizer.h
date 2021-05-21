#pragma once

#include "system/tasking/Task.h"

namespace Kernel
{

void finalize_task(Task *task);

void finalizer_initialize();

} // namespace Kernel
