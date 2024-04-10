#include <hideo-base/scafold.h>
#include <hideo-keyboard/views.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "app.h"

namespace Hideo::Shell {

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child indicator(Media::Icon icon) {
    return Ui::icon(icon) |
           Ui::center() |
           Ui::spacing(4);
}

Ui::Child statusbar(State const &state) {
    auto [_, time] = state.dateTime;
    return Ui::hflow(
               Ui::labelLarge("{02}:{02}", time.hour, time.minute) | Ui::center(),
               Ui::grow(NONE),
               indicator(Mdi::WIFI_STRENGTH_4),
               indicator(Mdi::NETWORK_STRENGTH_4),
               indicator(Mdi::BATTERY),
               Ui::labelLarge("100%") | Ui::center()
           ) |
           Ui::minSize({
               Ui::UNCONSTRAINED,
               36,
           }) |
           Ui::box({
               .padding = {12, 0},
               .backgroundPaint = Ui::GRAY900,
           });
}

Ui::Child statusbarButton(State const &state) {
    return Ui::button(
        Model::bind<Activate>(Panel::SYS),
        statusbar(state)
    );
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar(State const &) {
    return Ui::buttonHandle(
               Model::bind<Activate>(Panel::APPS)
           ) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

/* --- Taskbar -------------------------------------------------------------- */

Ui::Child taskbar(State const &state) {
    auto appsButton = Ui::button(
        Model::bind<Activate>(Panel::APPS),
        Ui::ButtonStyle::subtle(),
        Mdi::APPS, "Applications"
    );

    auto [date, time] = state.dateTime;
    auto dateTime = Io::format(
                        "{}. {} {}, {02}:{02}",
                        Io::toCapitalCase(date.month.abbr()),
                        date.dayOfMonth() + 1,
                        date.year.val(),
                        time.hour,
                        time.minute
    )
                        .unwrap();
    auto calButton = Ui::button(
        Model::bind<Activate>(Panel::NOTIS),
        Ui::ButtonStyle::subtle(),
        Mdi::CALENDAR,
        dateTime
    );

    auto trayButton = Ui::button(
        Model::bind<Activate>(Panel::SYS),
        Ui::ButtonStyle::subtle(),
        Ui::hflow(
            6,
            Layout::Align::CENTER,
            Ui::icon(Mdi::WIFI_STRENGTH_4),
            Ui::icon(Mdi::VOLUME_HIGH),
            Ui::icon(Mdi::BATTERY),
            Ui::labelMedium("100%")
        ) |

            Ui::center() |
            Ui::spacing({12, 6}) |
            Ui::bound()
    );

    return Ui::vflow(
        Ui::hflow(
            6,
            appsButton,
            calButton |
                Ui::center() |
                Ui::grow(),
            Ui::button(
                Keyboard::show,
                Ui::ButtonStyle::subtle(),
                Mdi::KEYBOARD
            ),
            trayButton
        ) |
            Ui::box({
                .padding = 6,
                .backgroundPaint = Ui::GRAY950.withOpacity(0.8),
            }),
        Ui::separator()
    );
}

/* --- Shells --------------------------------------------------------------- */

Ui::Child background(State const &state) {
    return Ui::image(state.background) |
           Ui::cover() |
           Ui::grow();
}

Ui::Child tabletPanels(State const &state) {
    return Ui::stack(
        state.activePanel == Panel::APPS
            ? appsFlyout(state)
            : Ui::empty(),
        state.activePanel == Panel::SYS
            ? sysFlyout(state)
            : Ui::empty()
    );
}

Ui::Child appHost(State const &state) {
    if (state.surfaces.len() == 0)
        return Ui::grow(NONE);

    auto &surface = state.surfaces[0];
    return Ui::empty() |
           Ui::box({.backgroundPaint = surface.color});
}

Ui::Child tablet(State const &state) {
    return Ui::stack(
        state.surfaces.len() == 0
            ? background(state)
            : Ui::empty(),
        Ui::vflow(
            Ui::vflow(
                statusbarButton(state),
                Ui::separator()
            ) |
                Ui::slideIn(Ui::SlideFrom::TOP),
            appHost(state) | Ui::grow(),
            navbar(state)
        )
    );
}

Ui::Child appStack(State const &state) {
    Ui::Children apps;
    usize index = state.surfaces.len() - 1;
    for (auto &s : iterRev(state.surfaces)) {
        apps.pushBack(
            scafold(Scafold{
                .icon = s.entry.icon.icon,
                .title = s.entry.name,
                .body = slot$(Ui::empty()),
            }) |
            Ui::box({
                .borderRadius = 6,
                .borderWidth = 1,
                .borderPaint = Ui::GRAY800,
                .backgroundPaint = Gfx::GRAY950,
                .shadowStyle = Gfx::BoxShadow::elevated(index ? 4 : 16),
            }) |
            Ui::placed(s.bound) |
            Ui::intent([=](Ui::Node &n, Sys::Event &e) {
                if (auto *m = e.is<Ui::DragEvent>(Sys::Propagation::UP)) {
                    e.accept();
                    Model::bubble<MoveApp>(n, {index, m->delta});
                } else if (auto *c = e.is<Events::RequestExitEvent>(Sys::Propagation::UP)) {
                    e.accept();
                    Model::bubble<CloseApp>(n, {index});
                }
            })
        );
        index--;
    }

    return Ui::stack(apps);
}

Ui::Child desktopPanels(State const &state) {
    return Ui::stack(
               state.activePanel == Panel::APPS
                   ? appsPanel(state) |
                         Ui::align(Layout::Align::START | Layout::Align::TOP) |
                         Ui::slideIn(Ui::SlideFrom::TOP)
                   : Ui::empty(),
               state.activePanel == Panel::NOTIS
                   ? notiPanel(state) |
                         Ui::align(Layout::Align::HCENTER | Layout::Align::TOP) |
                         Ui::slideIn(Ui::SlideFrom::TOP)
                   : Ui::empty(),
               state.activePanel == Panel::SYS
                   ? sysPanel(state) |
                         Ui::align(Layout::Align::END | Layout::Align::TOP) |
                         Ui::slideIn(Ui::SlideFrom::TOP)
                   : Ui::empty()
           ) |
           Ui::spacing({8, 38});
}

Ui::Child desktop(State const &state) {
    return Ui::stack(
        background(state),
        Ui::vflow(
            taskbar(state) | Ui::slideIn(Ui::SlideFrom::TOP),
            appStack(state) | Ui::grow()
        )
    );
}

Ui::Child app(bool isMobile) {
    return Ui::reducer<Model>(
        Shell::State{
            .isMobile = isMobile,
            .dateTime = Sys::dateTime(),
            .background = Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap(),
            .noti = {
                {
                    1,
                    {
                        {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                        "About"s,
                    },
                    "Hello"s,
                    "Hello, world!"s,
                },
                {
                    2,
                    {
                        {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                        "About"s,
                    },
                    "Hello"s,
                    "Hello, world!"s,
                },
                {
                    3,
                    {
                        {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                        "About"s,
                    },
                    "Hello"s,
                    "Hello, world!"s,
                },
                {
                    4,
                    {
                        {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                        "About"s,
                    },
                    "Hello"s,
                    "Hello, world!"s,
                },
            },
            .entries = {
                {
                    {Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP},
                    "About"s,
                },
                {
                    {Mdi::CALCULATOR, Gfx::ORANGE_RAMP},
                    "Calculator"s,
                },
                {
                    {Mdi::PALETTE_SWATCH, Gfx::RED_RAMP},
                    "Color Picker"s,
                },
                {
                    {Mdi::COUNTER, Gfx::GREEN_RAMP},
                    "Counter"s,
                },
                {
                    {Mdi::DUCK, Gfx::YELLOW_RAMP},
                    "Demos"s,
                },
                {
                    {Mdi::FILE, Gfx::ORANGE_RAMP},
                    "Files"s,
                },
                {
                    {Mdi::FORMAT_FONT, Gfx::BLUE_RAMP},
                    "Fonts"s,
                },
                {
                    {Mdi::EMOTICON, Gfx::RED_RAMP},
                    "Hello World"s,
                },
                {
                    {Mdi::IMAGE, Gfx::GREEN_RAMP},
                    "Icons"s,
                },
                {
                    {Mdi::IMAGE, Gfx::YELLOW_RAMP},
                    "Image Viewer"s,
                },
                {
                    {Mdi::CUBE, Gfx::BLUE_RAMP},
                    "Ray Tracer"s,
                },
                {
                    {Mdi::COG, Gfx::ZINC_RAMP},
                    "Settings"s,
                },
                {
                    {Mdi::TABLE, Gfx::GREEN_RAMP},
                    "Spreadsheet"s,
                },
                {
                    {Mdi::WIDGETS, Gfx::BLUE_RAMP},
                    "Widget Gallery"s,
                },
            },
        },
        [](auto state) {
            return Ui::stack(
                       state.locked
                           ? lock(state)
                           : (state.isMobile ? tablet(state)
                                             : desktop(state)),

                       state.isMobile
                           ? tabletPanels(state)
                           : desktopPanels(state)
                   ) |

                   Ui::dialogLayer() |
                   Ui::pinSize(
                       state.isMobile ? Math::Vec2i{411, 731}
                                      : Math::Vec2i{1280, 720}
                   );
        }
    );
}

} // namespace Hideo::Shell

Res<> entryPoint(Sys::Ctx &ctx) {
    auto args = useArgs(ctx);
    bool isMobile = Sys::useFormFactor() == Sys::FormFactor::MOBILE;
    return Ui::runApp(ctx, Hideo::Shell::app(isMobile));
}
