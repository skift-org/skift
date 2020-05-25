#pragma once

#include "kernel/node/Node.h"

typedef void (*DispatcherInteruptHandler)(void);

void dispatcher_initialize(void);

void dispatcher_dispatch(int interrupt);

void dispatcher_service(void);

void dispatcher_register_handler(int interrupt, DispatcherInteruptHandler handler);

void dispatcher_unregister_handler(DispatcherInteruptHandler handler);
