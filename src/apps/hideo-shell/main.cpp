#include <hideo-keyboard/views.h>
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
               Ui::labelLarge("22:07") | Ui::center(),
               Ui::grow(NONE),
               indicator(Mdi::WIFI_STRENGTH_4),
               indicator(Mdi::NETWORK_STRENGTH_4),
               indicator(Mdi::BATTERY),
               Ui::labelLarge("100%") | Ui::center()) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::box({.padding = {12, 0}, .backgroundPaint = Ui::GRAY900});
}

Ui::Child statusbarButton(State const &) {
    return Ui::button(
        Model::bind<Activate>(Panel::SYS),
        statusbar());
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar(State const &state) {
    return state.activePanel != Panel::NIL ? Ui::empty() : Ui::buttonHandle(Model::bind<Activate>(Panel::APPS)) | Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

/* --- Taskbar -------------------------------------------------------------- */

Ui::Child taskbar(State const &) {
    auto appsButton = Ui::button(
        Model::bind<Activate>(Panel::APPS),
        Ui::ButtonStyle::subtle(),
        Mdi::APPS, "Applications");

    auto calButton = Ui::button(
        Model::bind<Activate>(Panel::NOTIS),
        Ui::ButtonStyle::subtle(),
        Mdi::CALENDAR, "Jan. 12 2021, 22:07");

    auto trayButton = Ui::button(
        Model::bind<Activate>(Panel::SYS),
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
               Ui::button(Keyboard::show, Ui::ButtonStyle::subtle(), Mdi::KEYBOARD),
               trayButton) |
           Ui::box(Ui::BoxStyle{
               .padding = 6,
               .backgroundPaint = Ui::GRAY950.withOpacity(0.8),
           });
}

/* --- Shells --------------------------------------------------------------- */

Ui::Child tabletPanels(State const &state) {
    return Ui::stack(
        state.activePanel == Panel::APPS ? appsFlyout() : Ui::empty(),
        state.activePanel == Panel::SYS ? sysFlyout(state) : Ui::empty());
}

Ui::Child tablet(State const &state) {
    return Ui::vflow(
        Ui::vflow(
            statusbarButton(state),
            Ui::separator()) |
            Ui::slideIn(Ui::SlideFrom::TOP),
        Ui::grow(NONE),
        navbar(state));
}

Ui::Child desktopPanels(State const &state) {
    return Ui::stack(
               state.activePanel == Panel::APPS ? appsPanel() | Ui::align(Layout::Align::START | Layout::Align::TOP) | Ui::slideIn(Ui::SlideFrom::TOP) : Ui::empty(),
               state.activePanel == Panel::NOTIS ? notiPanel(state) | Ui::align(Layout::Align::HCENTER | Layout::Align::TOP) | Ui::slideIn(Ui::SlideFrom::TOP) : Ui::empty(),
               state.activePanel == Panel::SYS ? sysPanel(state) | Ui::align(Layout::Align::END | Layout::Align::TOP) | Ui::slideIn(Ui::SlideFrom::TOP) : Ui::empty()) |
           Ui::spacing({8, 38});
}

Ui::Child desktop(State const &state) {
    return Ui::vflow(
        Ui::vflow(
            taskbar(state),
            Ui::separator()) |
            Ui::slideIn(Ui::SlideFrom::TOP),
        Ui::grow(NONE));
}

Ui::Child app(bool isMobile) {
    return Ui::reducer<Model>({.isMobile = isMobile}, [](auto state) {
        auto wallpapers = Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap();
        auto background = Ui::align(Layout::Align::COVER, Ui::image(wallpapers));

        return Ui::dialogLayer(
            Ui::pinSize(
                state.isMobile ? Math::Vec2i{411, 731} : Math::Vec2i{1280, 1024},
                Ui::stack(
                    background,
                    state.locked ? lock(state)
                                 : (state.isMobile ? tablet(state)
                                                   : desktop(state)),
                    state.isMobile ? tabletPanels(state)
                                   : desktopPanels(state))));
    });
}

} // namespace Shell

Res<> entryPoint(Ctx &ctx) {
    auto args = useArgs(ctx);
    bool isMobile = useFormFactor() == FormFactor::MOBILE;
    return Ui::runApp(ctx, Shell::app(isMobile));
}
