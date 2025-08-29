#pragma once

import Karm.Core;

#include <hal/vmm.h>
#include <handover/spec.h>
#include <hjert-api/types.h>

#include "context.h"

namespace Hjert::Core {

struct Cpu;

struct Task;

struct Space;

} // namespace Hjert::Core

namespace Hjert::Arch {

struct Frame;

Core::Cpu& globalCpu();

Hal::Vmm& globalVmm();

Io::TextWriter& globalOut();

Res<Box<Core::Context>> createContext(Core::Mode mode, usize ip, usize sp, usize ksp, Hj::Args args);

Res<Arc<Hal::Vmm>> createVmm();

Res<> init(Handover::Payload&);

[[noreturn]] void stop();

void yield();

} // namespace Hjert::Arch
