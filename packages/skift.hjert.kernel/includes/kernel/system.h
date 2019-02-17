#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <skift/generic.h>
#include <skift/logger.h>

#include "kernel/processor.h"

void __panic(const char* package, const char* file, const char* function, const int line, processor_context_t * context, string message, ...);

#define STOP while(1){ cli(); hlt(); }
#define PANIC(x...) __panic(__PACKAGE__, __FILENAME__, (string)__FUNCTION__, __LINE__, NULL, x)
#define CPANIC(ctx, x...) __panic(__PACKAGE__, __FILENAME__, (string)__FUNCTION__, __LINE__, ctx, x)
#define setup(x, arg...) { sk_log(LOG_INFO, "Setting up " #x "..."); x##_setup(arg); }
