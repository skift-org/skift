#include <hideo-base/scafold.h>
#include <hideo-keyboard/views.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "app.h"

namespace Hideo::Shell {

// MARK: Status Bar ------------------------------------------------------------

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

// MARK: Navigation Bar --------------------------------------------------------

Ui::Child navbar(State const &) {
    return Ui::buttonHandle(
               Model::bind<Activate>(Panel::APPS)
           ) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

// MARK: Taskbar ---------------------------------------------------------------

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
            Math::Align::CENTER,
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
           ) |
           Ui::backgroundFilter(Gfx::BlurFilter{16});
}

// MARK: Shells ----------------------------------------------------------------

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
    if (state.instances.len() == 0)
        return Ui::grow(NONE);

    auto surface = state.instances[0];
    return Ui::empty() |
           Ui::box({.backgroundPaint = surface->color});
}

Ui::Child tablet(State const &state) {
    return Ui::stack(
        state.instances.len() == 0
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
    usize index = state.instances.len() - 1;
    for (auto &s : iterRev(state.instances)) {
        apps.pushBack(
            scafold({
                .icon = s->manifest->icon,
                .title = s->manifest->name,
                .body = slot$(Ui::empty()),
            }) |
            Ui::box({
                .borderRadii = 6,
                .borderWidth = 1,
                .borderPaint = Ui::GRAY800,
                .backgroundPaint = Ui::GRAY900,
                .shadowStyle = Gfx::BoxShadow::elevated(index ? 4 : 16),
            }) |
            Ui::placed(s->bound) |
            Ui::intent([=](Ui::Node &n, Sys::Event &e) {
                if (auto *m = e.is<Ui::DragEvent>()) {
                    e.accept();
                    Model::bubble<MoveInstance>(n, {index, m->delta});
                } else if (auto *c = e.is<Events::RequestExitEvent>()) {
                    e.accept();
                    Model::bubble<CloseInstance>(n, {index});
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
                         Ui::align(Math::Align::START | Math::Align::TOP) |
                         Ui::slideIn(Ui::SlideFrom::TOP)
                   : Ui::empty(),
               state.activePanel == Panel::NOTIS
                   ? notiPanel(state) |
                         Ui::align(Math::Align::HCENTER | Math::Align::TOP) |
                         Ui::slideIn(Ui::SlideFrom::TOP)
                   : Ui::empty(),
               state.activePanel == Panel::SYS
                   ? sysPanel(state) |
                         Ui::align(Math::Align::END | Math::Align::TOP) |
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

Ui::Child app(State state) {
    return Ui::reducer<Model>(
        std::move(state),
        [](auto const &state) {
            auto content =
                Ui::stack(
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

            if (state.nightLight) {
                content = Ui::foregroundFilter(Gfx::SepiaFilter{0.7}, content);
            }

            return content;
        }
    );
}

} // namespace Hideo::Shell
