#pragma once

import Karm.Core;

using namespace Karm;

namespace Efi {

struct Guid {
    u32 a{};
    u16 b{};
    u16 c{};
    Array<u8, 8> d{};

    auto operator<=>(Guid const&) const = default;
};

} // namespace Efi
