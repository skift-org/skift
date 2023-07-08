#pragma once

#include <hal/vmm.h>
#include <handover/spec.h>
#include <hjert-api/types.h>
#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-io/traits.h>

namespace Hjert::Core {

enum struct TaskMode : u8 {
    IDLE,  // The task is only run when there is no other task to run
    USER,  // The task is running in user mode
    SUPER, // The task is running in supervisor mode propably serving a syscall
};

struct Cpu;

struct Ctx;

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

Res<Box<Core::Ctx>> createCtx(Core::TaskMode mode, usize ip, usize sp, usize ksp, Hj::Args args);

Res<Strong<Hal::Vmm>> createVmm();

void yield();

} // namespace Hjert::Arch
