#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#include "kernel/cpu/cpu.h"

typedef u32 esp_t;
typedef esp_t (*irq_handler_t)(esp_t, context_t *);

void irq_setup();
irq_handler_t irq_register(int index, irq_handler_t handler);