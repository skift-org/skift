#pragma once

#include <hal/vmm.h>
#include <karm-base/range.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>

namespace Loader::Fw {

Strong<Hal::Vmm> createVmm();

} // namespace Loader::Fw
