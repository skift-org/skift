#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "error-page.h"

namespace Karm::Kira {

Ui::Child errorPageTitle(Mdi::Icon icon, String text) {
    return Ui::vflow(
        0,
        Math::Align::CENTER,
        Ui::icon(icon, 48) | Ui::insets(16),
        Ui::titleLarge(text)
    );
}

Ui::Child errorPageSubTitle(String text) {
    return Ui::titleMedium(text);
}

Ui::Child errorPageBody(String text) {
    return Ui::bodyMedium(text);
}

Ui::Child errorPageContent(Ui::Children children) {
    return Ui::vflow(
               6,
               Math::Align::CENTER,
               children
           ) |
           Ui::box({
               .foregroundFill = Ui::GRAY500,
           }) |
           Ui::center();
    ;
}

Ui::Child errorPageFooter(Ui::Children children) {
    return Ui::hflow(16, children) |
           Ui::insets({8, 0, 0, 0});
}

Ui::Child errorPage(Mdi::Icon icon, String text, String body) {
    return errorPageContent({
        errorPageTitle(icon, "An error occurred."s),
        errorPageSubTitle(text),
        errorPageBody(body),
    });
}

} // namespace Karm::Kira
