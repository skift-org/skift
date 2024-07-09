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

    bool isItalic() const {
        return get<ItalicAngle>().value != 0z;
    }
};

} // namespace Ttf
