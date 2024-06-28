
#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "alert.h"

namespace Hideo {

Ui::Child alert(String title, String body) {
    return Ui::vflow(
               0,
               Math::Align::CENTER,
               Ui::icon(Mdi::ALERT_DECAGRAM, 48) | Ui::spacing(16),
               Ui::titleLarge(title),
               Ui::bodyMedium(body)
           ) |
           Ui::box({
               .foregroundPaint = Ui::GRAY500,
           }) |
           Ui::center();
}

} // namespace Hideo
