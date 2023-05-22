#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>

#include "app.h"

namespace Shell {

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child systemTray(State const &);

Ui::Child indicator(Media::Icon icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

Ui::Child statusbar() {
    return Ui::box(
        {
            .backgroundPaint = Gfx::ZINC900,
        },
        Ui::minSize({Ui::UNCONSTRAINED, 36}, Ui::spacing({12, 0}, Ui::hflow(4, Ui::center(Ui::text(Ui::TextStyle::labelMedium(), "22:07")), Ui::grow(NONE), indicator(Mdi::NETWORK_STRENGTH_4), indicator(Mdi::WIFI_STRENGTH_4), indicator(Mdi::BATTERY)))));
}

Ui::Child statusbarButton(State const &state) {
    return Ui::button(
        [&](Ui::Node &n) {
            Ui::showDialog(n, systemTray(state));
        },
        statusbar());
}

/* --- System Tray ---------------------------------------------------------- */

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

Ui::Child systemTray(State const &state) {
    return Ui::vflow(
        statusbar(),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::TOP,
                0.3,
                Ui::box(
                    {
                        .borderRadius = {0, 0, 8, 8},
                        .backgroundPaint = Ui::GRAY950,
                    },
                    Ui::vflow(
                        quickSettings(state),
                        Ui::separator(),
                        notifications() | Ui::grow(),
                        Ui::dragHandle())))),
        Ui::empty(16));
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar() {
    return Ui::buttonHandle([](Ui::Node &n) {
        Ui::showDialog(n, appsFlyout());
    });
}

/* --- Taskbar -------------------------------------------------------------- */

Ui::Child taskbar(State const &) {
    auto appsButton = Ui::button(
        Model::bind<Activate>(Panel::APPS),
        Ui::ButtonStyle::subtle(),
        Mdi::APPS, "Applications");

    auto calButton = Ui::button(
        Model::bind<Activate>(Panel::CALENDAR),
        Ui::ButtonStyle::subtle(),
        Mdi::CALENDAR, "Jan. 12 2021, 22:07");

    auto trayButton = Ui::button(
        Model::bind<Activate>(Panel::NOTIS),
        Ui::ButtonStyle::subtle(),
        Ui::hflow(
            6,
            Layout::Align::CENTER,
            Ui::icon(Mdi::WIFI_STRENGTH_4),
            Ui::icon(Mdi::VOLUME_HIGH),
            Ui::icon(Mdi::BATTERY),
            Ui::labelMedium("100%")) |

            Ui::center() |
            Ui::spacing({12, 6}) |
            Ui::bound());

    return Ui::hflow(
               6,
               appsButton,
               calButton | Ui::center() | Ui::grow(),
               trayButton) |
           Ui::box(Ui::BoxStyle{
               .padding = 6,
               .backgroundPaint = Ui::GRAY950.withOpacity(0.8),
           }) |
           Ui::backgroundFilter(Gfx::BlurFilter{64});
}

Ui::Child panels(State const &state) {
    return Ui::stack(
               state.activePanel == Panel::APPS ? appsPanel() | Ui::align(Layout::Align::START | Layout::Align::TOP) : Ui::empty(),
               state.activePanel == Panel::CALENDAR ? Ui::empty() | panel() | Ui::align(Layout::Align::HCENTER | Layout::Align::TOP) : Ui::empty(),
               state.activePanel == Panel::NOTIS ? Ui::empty() | panel() | Ui::align(Layout::Align::END | Layout::Align::TOP) : Ui::empty()) |
           Ui::spacing({16, 38});
}

/* --- Shells --------------------------------------------------------------- */

Ui::Child tablet(State const &state) {
    return Ui::vflow(
        statusbarButton(state),
        Ui::grow(NONE),
        navbar());
}

Ui::Child desktop(State const &state) {
    return Ui::vflow(
        taskbar(state),
        Ui::separator(),
        Ui::grow(NONE));
}

Ui::Child app(bool isTablet) {
    return Ui::reducer<Model>({.isTablet = isTablet}, [](auto state) {
        auto wallpapers = Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap();
        auto background = Ui::align(Layout::Align::COVER, Ui::image(wallpapers));

        return Ui::dialogLayer(
            Ui::pinSize(
                state.isTablet ? Math::Vec2i{411, 731} : Math::Vec2i{1280, 1024},
                Ui::stack(
                    background,
                    state.locked ? lock(state)
                                 : (state.isTablet ? tablet(state)
                                                   : desktop(state)),
                    panels(state))));
    });
}

} // namespace Shell

Res<> entryPoint(Ctx &ctx) {
    auto args = useArgs(ctx);
    bool isTablet = args.has("+tablet");
    return Ui::runApp(ctx, Shell::app(isTablet));
}
