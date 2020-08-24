#pragma once

#include <libsystem/Common.h>

constexpr int LAPIC_EOI = 0x00B0;

void lapic_found(uintptr_t address);

void lapic_initialize();
