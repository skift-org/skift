#pragma once

#include <karm-io/bscan.h>

#include "base.h"

namespace Ttf {

struct Post : public Io::BChunk {
    static constexpr Str SIG = "post";

    using ItalicAngle = Io::BField<Fixed, 4>;
    using IsFixedPitch = Io::BField<u32be, 12>;

    bool isFixedPitch() const {
        return get<IsFixedPitch>();
    }

    f64 italicAngle() const {
        return get<ItalicAngle>().asF64();
    }
};

} // namespace Ttf
