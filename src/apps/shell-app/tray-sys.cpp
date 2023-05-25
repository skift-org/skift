#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Shell {

Ui::Child quickSetting(Mdi::Icon icon) {
    return Ui::center(Ui::state<bool>(false, [icon](auto state) {
        return Ui::button(
            [state](Ui::Node &) mutable {
                state.update(not state.value());
            },
            (state.value()
                 ? Ui::ButtonStyle::primary().withForegroundPaint(Gfx::WHITE)
                 : Ui::ButtonStyle::secondary().withForegroundPaint(Ui::GRAY300))
                .withRadius(99),
            Ui::minSize(48, Ui::center(Ui::icon(icon, 26))));
    }));
}

Ui::Child quickSettings(State const &state) {
    return Ui::box(
        {
            .backgroundPaint = Ui::GRAY900,
        },
        Ui::spacing(
            8,
            Ui::hflow(
                12,
                quickSetting(Mdi::SWAP_VERTICAL),
                quickSetting(Mdi::WIFI_STRENGTH_4),
                quickSetting(Mdi::BLUETOOTH),
                quickSetting(Mdi::MAP_MARKER_OUTLINE),
                quickSetting(Mdi::FLASHLIGHT),
                Ui::button(Model::bind<ToggleTablet>(), Ui::ButtonStyle::secondary(), state.isTablet ? Mdi::CELLPHONE : Mdi::TABLET),
                quickSetting(Mdi::CHEVRON_DOWN) |
                    Ui::end() |
                    Ui::grow())));
}

Ui::Child notiWrapper(App app, Ui::Child inner) {
    return Ui::vflow(
        Ui::spacing(
            12,
            Ui::vflow(
                8,
                Ui::hflow(
                    4,
                    Ui::box({.foregroundPaint = app.color[4]},
                            Ui::icon(app.icon, 12)),
                    Ui::text(Ui::TextStyle::labelMedium().withColor(Ui::GRAY400), app.name)),
                inner)),
        Ui::separator());
}

Ui::Child notiMsg(String title, String body) {
    return Ui::vflow(
        6,
        Ui::hflow(Ui::text(Ui::TextStyle::labelLarge(), title)),
        Ui::text(Ui::TextStyle::labelMedium(), body));
}

Ui::Child notification(Mdi::Icon icon, String title, String subtitle) {
    return notiWrapper({icon, Gfx::BLUE_RAMP, "Hello, world"}, notiMsg(title, subtitle));
}

Ui::Child notifications() {
    return Ui::vflow(
        Ui::spacing(
            {12, 0},
            Ui::hflow(
                Ui::center(Ui::text(Ui::TextStyle::labelMedium(), "Notifications")),
                Ui::grow(NONE),
                Ui::button(
                    Ui::NOP,
                    Ui::ButtonStyle::subtle(),
                    "Clear All"))),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"));
}

Ui::Child sys(State const &state) {
    return Ui::vflow(
        quickSettings(state),
        Ui::separator(),
        notifications() | Ui::grow());
}

Ui::Child sysPanel(State const &state) {
    return sys(state) | panel({400, 500});
}

Ui::Child sysFlyout(State const &state) {
    return Ui::vflow(
               sys(state) | Ui::grow(),
               Ui::dragHandle()) |
           Ui::box({
               .margin = {8, 8, 8, 8 + 64},
               .borderRadius = 8,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY800,
               .backgroundPaint = Ui::GRAY950,
           }) |
           Ui::bound() |
           Ui::dismisable(
               Ui::closeDialog,
               Ui::DismisDir::TOP,
               0.3);
}

} // namespace Shell
