#include <karm-main/base.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Settings {

Ui::Child tileButton(Ui::OnPress onPress, Mdi::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()) |
           Ui::spacing(8) |
           Ui::bound() |
           Ui::minSize({96, 72}) |
           Ui::button(std::move(onPress),
                      Ui::ButtonStyle::secondary());
}

Ui::Child pageHome(State const &) {
    Ui::Children items = {
        tileButton(Model::bind<GoTo>(Page::ACCOUNT), Mdi::ACCOUNT, "Accounts"),
        tileButton(Model::bind<GoTo>(Page::PERSONALIZATION), Mdi::PALETTE, "Personalization"),
        tileButton(Model::bind<GoTo>(Page::APPLICATIONS), Mdi::WIDGETS_OUTLINE, "Applications"),

        tileButton(Model::bind<GoTo>(Page::SYSTEM), Mdi::LAPTOP, "System"),
        tileButton(Model::bind<GoTo>(Page::NETWORK), Mdi::WIFI, "Network"),
        tileButton(Model::bind<GoTo>(Page::SECURITY), Mdi::SECURITY, "Security & Privacy"),

        tileButton(Model::bind<GoTo>(Page::UPDATES), Mdi::UPDATE, "Updates"),
        tileButton(Model::bind<GoTo>(Page::ABOUT), Mdi::INFORMATION_OUTLINE, "About"),
    };

    auto isMobile = useFormFactor() == FormFactor::MOBILE;

    auto gridLayout = Ui::GridStyle::simpleFixed({3, 96}, {3, 120}, 8);
    if (isMobile) {
        gridLayout = Ui::GridStyle::simpleGrow(4, 2, 4);
    }

    auto grid = Ui::grid(gridLayout, items) |
                Ui::spacing(8);

    if (isMobile) {
        return grid;
    }

    return grid | Ui::center() | Ui::bound() | Ui::grow();
}

} // namespace Settings
