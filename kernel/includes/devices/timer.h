#pragma once
#include "types.h"

void timer_setup();
u32 timer_get_ticks();
void timer_set_frequency(int hz);