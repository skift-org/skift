#pragma once

#include <hal/heap.h>
#include <hal/vmm.h>
#include <handover/spec.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Hjert {

struct Cpu;

} // namespace Hjert

namespace Hjert::Arch {

Cpu &cpu();

Error init(Handover::Payload &);

Hal::Vmm &vmm();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

} // namespace Hjert::Arch
