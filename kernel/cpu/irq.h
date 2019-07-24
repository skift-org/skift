#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include "processor.h"

typedef reg32_t (*irq_handler_t)(reg32_t, processor_context_t *);

void irq_setup();
irq_handler_t irq_register(int index, irq_handler_t handler);