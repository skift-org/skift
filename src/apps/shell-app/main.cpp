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

Ui::Child indicator(Media::Icon icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

Ui::Child statusbar() {
    return Ui::hflow(
               4,
               Ui::text(Ui::TextStyle::labelMedium(), "22:07") | Ui::center(),
               Ui::grow(NONE),
               indicator(Mdi::NETWORK_STRENGTH_4),
               indicator(Mdi::WIFI_STRENGTH_4),
               indicator(Mdi::BATTERY)) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::box({.padding = {12, 0}, .backgroundPaint = Gfx::ZINC900});
}

Ui::Child statusbarButton(State const &state) {
    return Ui::button(
        [&](Ui::Node &n) {
            Ui::showDialog(n, sysFlyout(state));
        },
        statusbar());
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
           });
}

Ui::Child panels(State const &state) {
    return Ui::stack(
               state.activePanel == Panel::APPS ? appsPanel() | Ui::align(Layout::Align::START | Layout::Align::TOP) : Ui::empty(),
               state.activePanel == Panel::CALENDAR ? Ui::empty() | panel() | Ui::align(Layout::Align::HCENTER | Layout::Align::TOP) : Ui::empty(),
               state.activePanel == Panel::NOTIS ? sysPanel(state) | Ui::align(Layout::Align::END | Layout::Align::TOP) : Ui::empty()) |
           Ui::spacing({8, 38});
}

/* --- Shells --------------------------------------------------------------- */

Ui::Child tablet(State const &state) {
    return Ui::vflow(
        statusbarButton(state),
        Ui::separator(),
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
