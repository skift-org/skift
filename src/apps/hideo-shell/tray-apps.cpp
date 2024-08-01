#include <hideo-keyboard/views.h>
#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Hideo::Shell {

Ui::Child searchInput() {
    return Ui::hflow(
               0,
               Math::Align::VCENTER | Math::Align::START,
               Ui::text(Ui::TextStyles::labelMedium().withColor(Ui::GRAY400), "Search…") | Ui::grow(),
               Ui::icon(Mdi::MAGNIFY, 24)
           ) |
           Ui::insets({12, 8}) |
           Ui::box({
               .borderRadii = 4,
               .borderWidth = 1,
               .backgroundFill = Ui::GRAY900,
           }) |
           Ui::button(Keyboard::show);
}

Ui::Child appIcon(Mdi::Icon const &icon, Gfx::ColorRamp ramp, isize size = 22) {
    return Ui::icon(icon, size) |
           Ui::insets(size / 2.75) |
           Ui::center() |
           Ui::box({
               .borderRadii = size * 0.25,
               .borderWidth = 1,
               .borderFill = ramp[5],
               .backgroundFill = ramp[6],
               .foregroundFill = ramp[1],
           });
}

Ui::Child appRow(Manifest const &manifest, usize i) {
    return Ui::ButtonStyle::subtle(),
           Ui::hflow(
               12,
               Math::Align::START | Math::Align::VCENTER,
               appIcon(manifest.icon, manifest.ramp),
               Ui::labelLarge(manifest.name)
           ) |
               Ui::insets(6) |
               Ui::button(Model::bind<StartInstance>(i), Ui::ButtonStyle::subtle());
}

Ui::Child appsList(State const &state) {
    return Ui::vflow(
        iter(state.manifests)
            .mapi([](auto &man, usize i) {
                return appRow(*man, i);
            })
            .collect<Ui::Children>()
    );
}

Ui::Child appTile(Manifest const &manifest, usize i) {
    return Ui::vflow(
               26,
               appIcon(manifest.icon, manifest.ramp, 26),
               Ui::labelLarge(manifest.name)
           ) |
           Ui::button(
               Model::bind<StartInstance>(i),
               Ui::ButtonStyle::subtle()
           );
}

Ui::Child appsGrid(State const &state) {
    return Ui::grid(
        Ui::GridStyle::simpleFixed({8, 64}, {4, 64}),
        iter(state.manifests)
            .mapi([](auto &man, usize i) {
                return appTile(*man, i);
            })
            .collect<Ui::Children>()
    );
}

Ui::Child runningApp(Instance const &, usize i) {
    return Ui::stack(
               Ui::empty() |
                   Ui::bound() |
                   Ui::button(Model::bind<FocusInstance>(i)),
               Ui::button(Model::bind<CloseInstance>(i), Ui::ButtonStyle::secondary(), Mdi::CLOSE) |
                   Ui::align(Math::Align::TOP_END) |
                   Ui::insets({0, 6, 6, 0})
           ) |
           Ui::pinSize({120, 192});
}

Ui::Child runningApps(State const &state) {
    if (state.instances.len() == 0)
        return Ui::empty(64);
    return Ui::hflow(
               8,
               iter(state.instances)
                   .mapi([](auto &instance, usize i) {
                       return runningApp(*instance, i);
                   })
                   .collect<Ui::Children>()
           ) |
           Ui::center() | Ui::insets({0, 64, 0, 16});
}

Ui::Child apps(State const &state) {
    return Ui::vflow(
        Ui::hflow(
            4,
            searchInput() | Ui::grow(),
            Ui::button(
                Model::bind<ToggleAppThumbnail>(true),
                state.isAppPanelThumbnails
                    ? Ui::ButtonStyle::secondary()
                    : Ui::ButtonStyle::subtle(),
                Mdi::VIEW_GRID
            ),

            Ui::button(
                Model::bind<ToggleAppThumbnail>(false),
                state.isAppPanelThumbnails
                    ? Ui::ButtonStyle::subtle()
                    : Ui::ButtonStyle::secondary(),
                Mdi::FORMAT_LIST_BULLETED_SQUARE
            )
        ),

        (state.isAppPanelThumbnails
             ? appsGrid(state)
             : appsList(state)) |
            Ui::insets({0, 12}) | Ui::vscroll() | Ui::grow()
    );
}

Ui::Child appsPanel(State const &state) {
    return apps(state) | panel();
}

Ui::Child appsFlyout(State const &state) {
    return Ui::vflow(
        runningApps(state),
        Ui::vflow(
            Ui::dragHandle(),
            apps(state) | Ui::grow()
        ) |
            Ui::box({
                .margin = {8, 8, 8, 8},
                .padding = {12, 0},
                .borderRadii = 8,
                .borderWidth = 1,
                .borderFill = Ui::GRAY800,
                .backgroundFill = Ui::GRAY950,
            }) |
            Ui::bound() |
            Ui::dismisable(
                Model::bind<Activate>(Panel::NIL),
                Ui::DismisDir::DOWN,
                0.3
            ) |
            Ui::slideIn(Ui::SlideFrom::BOTTOM) | Ui::grow()
    );
}

} // namespace Hideo::Shell
