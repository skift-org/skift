#pragma once

#include <karm-base/std.h>

namespace Efi {

struct Guid {
    u32 a{};
    u16 b{};
    u16 c{};
    u8 d[8]{};

    auto operator<=>(Guid const &) const = default;
};

} // namespace Efi
