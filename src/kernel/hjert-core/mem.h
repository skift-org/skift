#pragma once

#include <hal/heap.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <handover/spec.h>

namespace Hjert::Mem {

Error init(Handover::Payload &);

Hal::Heap &heap();

Hal::Pmm &pmm();

Hal::Vmm &vmm();

} // namespace Hjert::Mem
