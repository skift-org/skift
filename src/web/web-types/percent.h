#pragma once

#include <karm-base/distinct.h>
#include <karm-base/std.h>

namespace Web::Types {

using Percent = Distinct<f64, struct _PercentTag>;

template <typename T>
struct PercentOf {
    float _val;
};

template <typename T>
struct PercentOr {

    PercentOr(Percent) {
    }

    PercentOr(T) {
    }
};

} // namespace Web::Types
