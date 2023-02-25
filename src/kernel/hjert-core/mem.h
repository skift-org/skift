#pragma once

#include <hal/kmm.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <handover/spec.h>

namespace Hjert::Core {

namespace Mem {
Res<> init(Handover::Payload &);
} // namespace Mem

Hal::Kmm &kmm();

Hal::Pmm &pmm();

Hal::Vmm &vmm();

} // namespace Hjert::Core
