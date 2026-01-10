#pragma once

#include <hal/kmm.h>
#include <hal/pmm.h>
#include <hal/vmm.h>
#include <vaerk-handover/spec.h>

namespace Hjert::Core {

Res<> initMem(Handover::Payload&);

Hal::Kmm& kmm();

Hal::Pmm& pmm();

} // namespace Hjert::Core
