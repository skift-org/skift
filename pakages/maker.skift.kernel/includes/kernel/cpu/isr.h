#pragma once
#include "kernel/cpu/cpu.h"

typedef void (*isr_handler_t)(context_t *);

void isr_setup();
isr_handler_t isr_register(int index, isr_handler_t handler);