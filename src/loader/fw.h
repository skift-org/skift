#pragma once

#include <hal/vmm.h>
#include <handover/builder.h>
#include <karm-base/range.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>

namespace Loader::Fw {

Result<Strong<Hal::Vmm>> createVmm();

Error finalizeHandover(Handover::Builder &builder);

} // namespace Loader::Fw
