#pragma once

#include <libsystem/Common.h>

void lapic_found(uintptr_t address);

void lapic_initialize();

void lapic_ack();
