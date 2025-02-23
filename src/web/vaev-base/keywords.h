#pragma once

#include <karm-base/string.h>
#include <karm-io/emit.h>

namespace Vaev {

template <Karm::StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }
};

namespace Keywords {
using Auto = Keyword<"auto">;
} // namespace Keywords

} // namespace Vaev
