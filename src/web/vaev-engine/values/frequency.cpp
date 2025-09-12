export module Vaev.Engine:values.frequency;

import Karm.Core;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-values/#frequency
// It represents the number of occurrences per second.
export struct Frequency : Distinct<f64, struct FrequencyTag> {
    using Distinct::Distinct;

    static constexpr Frequency fromHz(f64 hz) {
        return Frequency{hz};
    }

    static constexpr Frequency fromKHz(f64 khz) {
        return Frequency{khz * 1e3};
    }

    void repr(Io::Emit& e) const {
        if (value() >= 1e3) {
            e("{}kHz", value() / 1e3);
        } else {
            e("{}Hz", value());
        }
    }
};

} // namespace Vaev
