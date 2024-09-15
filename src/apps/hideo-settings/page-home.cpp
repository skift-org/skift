#include <karm-app/form-factor.h>
#include <karm-sys/context.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child tileButton(Ui::OnPress onPress, Mdi::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()
           ) |
           Ui::insets(8) |
           Ui::bound() |
           Ui::minSize({96, 72}) |
           Ui::button(std::move(onPress), Ui::ButtonStyle::secondary());
}

Ui::Child pageHome(State const &) {
    Ui::Children items = {
        tileButton(Model::bind<GoTo>(Page::ACCOUNT), Mdi::ACCOUNT, "Accounts"s),
        tileButton(Model::bind<GoTo>(Page::PERSONALIZATION), Mdi::PALETTE, "Personalization"s),
        tileButton(Model::bind<GoTo>(Page::APPLICATIONS), Mdi::WIDGETS_OUTLINE, "Applications"s),

        tileButton(Model::bind<GoTo>(Page::SYSTEM), Mdi::LAPTOP, "System"s),
        tileButton(Model::bind<GoTo>(Page::NETWORK), Mdi::WIFI, "Network"s),
        tileButton(Model::bind<GoTo>(Page::SECURITY), Mdi::SECURITY, "Security & Privacy"s),

        tileButton(Model::bind<GoTo>(Page::UPDATES), Mdi::UPDATE, "Updates"s),
        tileButton(Model::bind<GoTo>(Page::ABOUT), Mdi::INFORMATION_OUTLINE, "About"s),
    };

    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;

    auto gridLayout = isMobile
                          ? Ui::GridStyle::simpleGrow(4, 2, 4)
                          : Ui::GridStyle::simpleFixed({3, 96}, {3, 120}, 8);

    auto grid = Ui::grid(gridLayout, items) |
                Ui::insets(8);

    if (isMobile)
        return grid;

    return grid | Ui::center() | Ui::bound() | Ui::grow();
}

} // namespace Hideo::Settings
