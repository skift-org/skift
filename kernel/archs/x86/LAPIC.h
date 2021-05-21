#pragma once

#include <libsystem/Common.h>

namespace Arch::x86
{

void lapic_found(uintptr_t address);

void lapic_initialize();

void lapic_ack();

} // namespace Arch::x86
