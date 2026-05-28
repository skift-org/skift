module;

#include <hal/vmm.h>
#include <vaerk-handover/spec.h>

export module Opstart:arch;

import Karm.Core;

using namespace Karm;

extern "C" void __enterKernel(usize entry, usize payload, usize stack, usize vmm);

namespace Opstart::Arch {

export void enterKernel(usize entry, Handover::Payload& payload, usize stack, Hal::Vmm& vmm) {
    __enterKernel(
        entry,
        reinterpret_cast<usize>(&payload) + Handover::KERNEL_BASE,
        stack,
        vmm.root()
    );
}

} // namespace Opstart::Arch