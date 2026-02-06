module;

#include <hal/vmm.h>
#include <vaerk-handover/spec.h>

export module Hjert.Core:arch;

import Karm.Core;
import Hjert.Api;

namespace Hjert::Core {

export struct Cpu;
export struct Task;
export struct Space;
export struct Context;

} // namespace Hjert::Core

namespace Hjert::Arch {

export struct Frame;

export Core::Cpu& globalCpu();

export Hal::Vmm& globalVmm();

export Io::TextWriter& globalOut();

export Res<Box<Core::Context>> createContext(Hj::Mode mode, usize ip, usize sp, usize ksp, Hj::Args args);

export Res<Arc<Hal::Vmm>> createVmm();

export Res<> init(Handover::Payload&);

export [[noreturn]] void stop();

export void yield();

} // namespace Hjert::Arch
