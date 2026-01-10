#pragma once

import Karm.Core;

#include <hal/vmm.h>
#include <vaerk-handover/builder.h>

namespace Opstart::Fw {

Res<Arc<Hal::Vmm>> createVmm();

Res<> finalizeHandover(Handover::Builder& builder);

void enterKernel(usize entry, Handover::Payload& payload, usize stack, Hal::Vmm& vmm);

Hal::PmmRange imageRange();

} // namespace Opstart::Fw
