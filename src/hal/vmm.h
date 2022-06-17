#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/result.h>

#include "pmm.h"

namespace Hal {

enum struct VmmFlags : uint64_t {
    NIL = 0,
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    USER = (1 << 3),
    GLOBAL = (1 << 4),
    WRITETHROUGH = (1 << 5),
};

FlagsEnum$(VmmFlags);

struct VmmRange : public USizeRange {
    using USizeRange::USizeRange;
};

struct Vmm {
    using enum VmmFlags;
    using Flags = VmmFlags;
    using Range = VmmRange;

    virtual ~Vmm() = default;

    virtual Error map(VmmRange vaddr, PmmRange paddr, VmmFlags flags) = 0;

    virtual Error unmap(VmmRange vaddr) = 0;

    virtual Error update(VmmRange vaddr, VmmFlags flags) = 0;

    virtual Error flush(VmmRange vaddr) = 0;

    virtual void dump() = 0;

    virtual void activate() = 0;
};

} // namespace Hal
