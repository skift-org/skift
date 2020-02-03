#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#include "processor.h"

typedef void (*isr_handler_t)(processor_context_t *);

void isr_setup();
isr_handler_t isr_register(int index, isr_handler_t handler);