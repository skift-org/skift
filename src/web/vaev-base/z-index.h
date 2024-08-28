#pragma once

#include <karm-io/emit.h>

namespace Vaev {

struct ZIndex {
    enum struct _Auto {
        AUTO,
    };

    using enum _Auto;

    bool auto_ = true;
    isize value = 0;

    constexpr ZIndex(_Auto) : auto_(true) {}

    constexpr ZIndex(isize value) : auto_(false), value(value) {}

    bool operator==(_Auto) const { return auto_; }

    void repr(Io::Emit &e) const {
        if (auto_) {
            e("auto");
        } else {
            e("{}", value);
        }
    }
};

} // namespace Vaev
