#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "cpu/cpu.h"
#include "kernel/logger.h"
#include <stdio.h>

void __panic(const string file, const string function, const int line, context_t * context, string message, ...);

#define STOP while(1){ cli(); hlt(); }
#define PANIC(x...) __panic(__FILE__, (string)__FUNCTION__, __LINE__, NULL, x)
#define CPANIC(ctx, x...) __panic(__FILE__, (string)__FUNCTION__, __LINE__, ctx, x)
#define setup(x, arg...) { log("Setting up " #x "..."); x##_setup(arg); }