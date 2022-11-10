#pragma once

#include <hal/heap.h>
#include <hal/vmm.h>
#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Hjert::Arch {

Error init();

Hal::Pmm &pmm();

Hal::Vmm &vmm();

Hal::Heap &heap();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

[[noreturn]] void idleCpu();

} // namespace Hjert::Arch
