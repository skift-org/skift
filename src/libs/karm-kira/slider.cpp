#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "slider.h"

namespace Karm::Kira {

Ui::Child slider(f64 value, Ui::OnChange<f64> onChange, Mdi::Icon icon, Str text) {
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
               .backgroundPaint = Ui::ACCENT600,
           }) |
           Ui::dragRegion() |
           Ui::slider(value, std::move(onChange)) |
           Ui::box({
               .borderRadii = 6,
               .backgroundPaint = Ui::GRAY900,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 36});
}

} // namespace Karm::Kira
