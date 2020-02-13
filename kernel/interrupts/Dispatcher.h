#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/node/Node.h"

typedef void (*DispatcherInteruptHandler)(void);

void dispatcher_initialize(void);

void dispatcher_dispatch(int interrupt);

void dispatcher_service(void);

void dispatcher_register_handler(int interrupt, DispatcherInteruptHandler handler);

void dispatcher_unregister_handler(DispatcherInteruptHandler handler);
