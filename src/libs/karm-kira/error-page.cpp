module;

#include <karm-gfx/icon.h>
#include <karm-math/align.h>

export module Karm.Kira:errorPage;

import Karm.Ui;

namespace Karm::Kira {

export Ui::Child errorPageTitle(Gfx::Icon icon, String text) {
    return Ui::vflow(
        0,
        Math::Align::CENTER,
        Ui::icon(icon, 48) | Ui::insets(16),
        Ui::titleLarge(text)
    );
}

export Ui::Child errorPageSubTitle(String text) {
    return Ui::titleMedium(text);
}

export Ui::Child errorPageBody(String text) {
    return Ui::bodyMedium(text);
}

export Ui::Child errorPageContent(Ui::Children children) {
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

export Ui::Child errorPageFooter(Ui::Children children) {
    return Ui::hflow(16, children) |
           Ui::insets({8, 0, 0, 0});
}

export Ui::Child errorPage(Gfx::Icon icon, String text, String body) {
    return errorPageContent({
        errorPageTitle(icon, "An error occurred."s),
        errorPageSubTitle(text),
        errorPageBody(body),
    });
}

} // namespace Karm::Kira
