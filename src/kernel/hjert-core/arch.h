#pragma once

#include <hal/heap.h>
#include <hal/vmm.h>
#include <handover/spec.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Hjert {

struct Cpu;

namespace Sched {

struct Task;

} // namespace Sched

} // namespace Hjert

namespace Hjert::Arch {

Cpu &cpu();

Error init(Handover::Payload &);

Hal::Vmm &vmm();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

void start(Sched::Task &, uintptr_t ip, uintptr_t sp, Array<uintptr_t, 5> args);

} // namespace Hjert::Arch
