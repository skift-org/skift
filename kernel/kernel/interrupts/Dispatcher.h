#pragma once

#include "kernel/node/Node.h"

typedef void (*DispatcherInteruptHandler)();

void dispatcher_initialize();

void dispatcher_dispatch(int interrupt);

void dispatcher_service();
