#pragma once

#include <hal/heap.h>
#include <hal/vmm.h>
#include <handover/spec.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Hjert::Arch {

Error init(Handover::Payload &);

Hal::Vmm &vmm();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

void relaxeCpu();

} // namespace Hjert::Arch
