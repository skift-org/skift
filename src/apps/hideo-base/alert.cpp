
#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "alert.h"

namespace Hideo {

Ui::Child alert(String title, String subtitle) {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               Ui::icon(Mdi::ALERT_DECAGRAM, 48),
               Ui::titleLarge(title),
               Ui::bodyMedium(subtitle)) |
           Ui::box({
               .foregroundPaint = Ui::GRAY500,
           }) |
           Ui::center();
}

} // namespace Hideo
