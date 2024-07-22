#pragma once

#include <karm-io/emit.h>

namespace Vaev {

struct ZIndex {
    enum struct _Auto {
        AUTO,
    };

    using enum _Auto;

    bool auto_;
    isize value;

    constexpr ZIndex(_Auto) : auto_(true) {}

    constexpr ZIndex(isize value) : auto_(false), value(value) {}

    void repr(Io::Emit &e) const {
        if (auto_) {
            e("auto");
        } else {
            e("{}", value);
        }
    }
};

} // namespace Vaev
