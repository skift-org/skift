#pragma once

#include <hal/vmm.h>
#include <handover/builder.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>

namespace Loader::Fw {

Result<Strong<Hal::Vmm>> createVmm();

Error finalizeHandover(Handover::Builder &builder);

void enterKernel(size_t entry, Handover::Payload &payload, size_t stack, Hal::Vmm &vmm);

Hal::PmmRange imageRange();

} // namespace Loader::Fw
