#pragma once

#include <hal/vmm.h>
#include <handover/spec.h>
#include <hjert-api/types.h>
#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-io/traits.h>

#include "ctx.h"

namespace Hjert::Core {

struct Cpu;

struct Task;

struct Space;

} // namespace Hjert::Core

namespace Hjert::Arch {

struct Frame;

Core::Cpu &cpu();

Res<> init(Handover::Payload &);

Hal::Vmm &vmm();

Io::TextWriter &loggerOut();

[[noreturn]] void stopAll();

Res<Box<Core::Ctx>> createCtx(Core::Mode mode, usize ip, usize sp, usize ksp, Hj::Args args);

Res<Strong<Hal::Vmm>> createVmm();

void yield();

} // namespace Hjert::Arch
