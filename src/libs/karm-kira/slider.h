#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child slider(f64 value, Ui::OnChange<f64> onChange, Mdi::Icon icon, Str text);

template <typename T>
static inline Ui::Child slider(T value, Range<T> range, Ui::OnChange<T> onChange, Mdi::Icon icon, Str text) {
    return slider(
        (value - range.start) / (f64)(range.end() - range.start),
        [=](Ui::Node& n, f64 v) {
            onChange(n, range.start + v * (range.end() - range.start));
        },
        icon,
        text
    );
}

} // namespace Karm::Kira
