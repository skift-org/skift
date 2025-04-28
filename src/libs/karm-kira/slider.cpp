module;

#include <karm-gfx/icon.h>
#include <karm-math/align.h>

export module Karm.Kira:slider;

import Karm.Ui;

namespace Karm::Kira {

export Ui::Child slider(f64 value, Ui::Send<f64> onChange, Gfx::Icon icon, Str text) {
    return Ui::hflow(
               0,
               Math::Align::CENTER,
               Ui::icon(icon) |
                   Ui::center() |
                   Ui::aspectRatio(1) |
                   Ui::bound(),
               Ui::labelMedium(text)
           ) |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = Ui::ACCENT600,
           }) |
           Ui::dragRegion() |
           Ui::slider(value, std::move(onChange)) |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = Ui::GRAY900,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 36});
}

export template <typename T>
Ui::Child slider(T value, Range<T> range, Ui::Send<T> onChange, Gfx::Icon icon, Str text) {
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
