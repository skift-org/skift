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

Ui::Child navbar(State const &) {
    return Ui::buttonHandle(
               Model::bind<Activate>(Panel::APPS)) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
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

Ui::Child background(State const &state) {
    return Ui::image(state.background) |
           Ui::cover() | Ui::grow();
}

Ui::Child tabletPanels(State const &state) {
    return Ui::stack(
        state.activePanel == Panel::APPS ? appsFlyout(state) : Ui::empty(),
        state.activePanel == Panel::SYS ? sysFlyout(state) : Ui::empty());
}

Ui::Child appHost(State const &state) {
    if (state.surfaces.len() == 0) {
        return Ui::grow(NONE);
    }

    auto &surface = state.surfaces[0];
    return Ui::empty() |
           Ui::box({.backgroundPaint = surface.color}) |
           Ui::grow();
}

Ui::Child tablet(State const &state) {
    return Ui::stack(
        state.surfaces.len() == 0 ? background(state) : Ui::empty(),
        Ui::vflow(
            Ui::vflow(
                statusbarButton(state),
                Ui::separator()) |
                Ui::slideIn(Ui::SlideFrom::TOP),
            appHost(state),
            navbar(state)));
}

Ui::Child desktopPanels(State const &state) {
    return Ui::stack(
               state.activePanel == Panel::APPS ? appsPanel(state) | Ui::align(Layout::Align::START | Layout::Align::TOP) | Ui::slideIn(Ui::SlideFrom::TOP) : Ui::empty(),
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
    return Ui::reducer<Model>(
        {
            .isMobile = isMobile,
            .background = Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap(),
            .noti = {
                {
                    1,
                    {{Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP}, "About"},
                    "Hello",
                    "Hello, world!",
                },
                {
                    2,
                    {{Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP}, "About"},
                    "Hello",
                    "Hello, world!",
                },
                {
                    3,
                    {{Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP}, "About"},
                    "Hello",
                    "Hello, world!",
                },
                {
                    4,
                    {{Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP}, "About"},
                    "Hello",
                    "Hello, world!",
                },
            },
            .entries = {
                {
                    {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                    "About",
                },
                {
                    {Mdi::CALCULATOR, Gfx::ORANGE_RAMP},
                    "Calculator",
                },
                {
                    {Mdi::PALETTE_SWATCH, Gfx::RED_RAMP},
                    "Color Picker",
                },
                {
                    {Mdi::COUNTER, Gfx::GREEN_RAMP},
                    "Counter",
                },
                {
                    {Mdi::DUCK, Gfx::YELLOW_RAMP},
                    "Demos",
                },
                {
                    {Mdi::FILE, Gfx::ORANGE_RAMP},
                    "Files",
                },
                {
                    {Mdi::FORMAT_FONT, Gfx::BLUE_RAMP},
                    "Fonts",
                },
                {
                    {Mdi::EMOTICON, Gfx::RED_RAMP},
                    "Hello World",
                },
                {
                    {Mdi::IMAGE, Gfx::GREEN_RAMP},
                    "Icons",
                },
                {
                    {Mdi::IMAGE, Gfx::YELLOW_RAMP},
                    "Image Viewer",
                },
                {
                    {Mdi::CUBE, Gfx::BLUE_RAMP},
                    "Ray Tracer",
                },
                {
                    {Mdi::COG, Gfx::ZINC_RAMP},
                    "Settings",
                },
                {
                    {Mdi::TABLE, Gfx::GREEN_RAMP},
                    "Spreadsheet",
                },
                {
                    {Mdi::WIDGETS, Gfx::BLUE_RAMP},
                    "Widget Gallery",
                },
            },
        },
        [](auto state) {
            return Ui::stack(
                       state.locked ? lock(state)
                                    : (state.isMobile ? tablet(state)
                                                      : desktop(state)),
                       state.isMobile ? tabletPanels(state)
                                      : desktopPanels(state)) |
                   Ui::dialogLayer() |
                   Ui::pinSize(
                       state.isMobile ? Math::Vec2i{411, 731}
                                      : Math::Vec2i{1280, 720});
        });
}

} // namespace Shell

Res<> entryPoint(Ctx &ctx) {
    auto args = useArgs(ctx);
    bool isMobile = useFormFactor() == FormFactor::MOBILE;
    return Ui::runApp(ctx, Shell::app(isMobile));
}
