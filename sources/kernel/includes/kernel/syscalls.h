#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "types.h"
#include "cpu/cpu.h"

void syscall_dispatcher(context_t *context);