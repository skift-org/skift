#pragma once

#include <hal/heap.h>
#include <hal/pmm.h>
#include <handover/spec.h>

namespace Hjert::Mem {

Error init(Handover::Payload &);

Hal::Heap &heap();

Hal::Pmm &pmm();

} // namespace Hjert::Mem
