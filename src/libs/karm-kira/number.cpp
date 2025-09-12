export module Karm.Kira:number;

import Karm.Core;
import Karm.Ui;
import Mdi;

namespace Karm::Kira {

export Ui::Child number(f64 value, Ui::Send<f64> onChange, f64 step) {
    return Ui::hflow(
               Ui::button(
                   [onChange, value, step](auto& n) {
                       onChange(n, value - step);
                   },
                   Ui::ButtonStyle::subtle(),
                   Mdi::MINUS
               ),
               Ui::labelMedium("{:.02}", value) | Ui::insets({0, 4}) | Ui::center(),
               Ui::button(
                   [onChange, value, step](auto& n) {
                       onChange(n, value + step);
                   },
                   Ui::ButtonStyle::subtle(),
                   Mdi::PLUS
               )
           ) |
           Ui::box({
               .borderRadii = 4,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
           }) |
           Ui::focusable();
}

} // namespace Karm::Kira
