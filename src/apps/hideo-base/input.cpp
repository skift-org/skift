#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "input.h"

namespace Hideo {

Ui::Child slider(f64 value, Ui::OnChange<f64> onChange, Mdi::Icon icon) {
    return Ui::hflow(
               Ui::grow(NONE),
               Ui::icon(icon) |
                   Ui::center() |
                   Ui::aspectRatio(1) |
                   Ui::bound() |
                   Ui::dragRegion()
           ) |
           Ui::box({
               .borderRadius = 6,
               .backgroundPaint = Ui::ACCENT600,
           }) |
           Ui::slider(value, std::move(onChange)) |
           Ui::box({
               .borderRadius = 6,
               .backgroundPaint = Ui::GRAY900,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 36});
}

} // namespace Hideo
