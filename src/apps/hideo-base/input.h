#pragma once

#include <karm-ui/input.h>

namespace Hideo {

Ui::Child slider(f64 value, Ui::OnChange<f64> onChange, Mdi::Icon icon);

template <typename T>
static inline Ui::Child slider(T value, Range<T> range, Ui::OnChange<T> onChange, Mdi::Icon icon) {
    return slider(
        (value - range.start) / (f64)(range.end() - range.start),
        [=](Ui::Node &n, f64 v) {
            onChange(n, range.start + v * (range.end() - range.start));
        },
        icon
    );
}

} // namespace Hideo
