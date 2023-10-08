#include <hideo-keyboard/views.h>
#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Shell {

Ui::Child searchInput() {
    return Ui::hflow(
               0,
               Layout::Align::VCENTER | Layout::Align::START,
               Ui::labelMedium("Search...") | Ui::grow(),
               Ui::icon(Mdi::MAGNIFY, 24)) |
           Ui::spacing({12, 8}) |
           Ui::box({
               .borderRadius = 4,
               .borderWidth = 1,
               .backgroundPaint = Ui::GRAY900,
           }) |
           Ui::button(Keyboard::show);
}

Ui::Child appIcon(MenuIcon const &icon, isize size = 22) {
    return Ui::icon(icon.icon, size) |
           Ui::spacing(size / 2.75) |
           Ui::center() |
           Ui::box({
               .borderRadius = size * 0.25,
               .borderWidth = 1,
               .borderPaint = icon.colors[5],
               .backgroundPaint = icon.colors[6],
               .foregroundPaint = icon.colors[1],
           });
}

Ui::Child appRow(MenuEntry const &entry, usize i) {
    return Ui::ButtonStyle::subtle(),
           Ui::hflow(
               12,
               Layout::Align::START | Layout::Align::VCENTER,
               appIcon(entry.icon),
               Ui::labelLarge(entry.name)) |
               Ui::spacing(6) | Ui::button(Model::bind<StartApp>(i), Ui::ButtonStyle::subtle());
}

Ui::Child runningApp(Surface const &surface, usize i) {
    return Ui::stack(
               appIcon(surface.entry.icon) |
                   Ui::bound() |
                   Ui::button(Model::bind<FocusApp>(i)),
               Ui::button(Model::bind<CloseApp>(i), Ui::ButtonStyle::secondary(), Mdi::CLOSE) |
                   Ui::align(Layout::Align::TOP_END) |
                   Ui::spacing({0, 6, 6, 0})) |
           Ui::pinSize({120, 192});
}

Ui::Child runningApps(State const &state) {
    if (state.surfaces.len() == 0)
        return Ui::empty(64);
    return Ui::hflow(
               8,
               iter(state.surfaces)
                   .mapi(runningApp)
                   .collect<Ui::Children>()) |
           Ui::center() | Ui::spacing({0, 64, 0, 16});
}

Ui::Child apps(Ui::Children apps) {
    return Ui::vflow(
        Ui::hflow(
            4,
            searchInput() | Ui::grow(),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::VIEW_GRID),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_LIST_BULLETED_SQUARE)),
        Ui::vflow(0, apps) |
            Ui::spacing({0, 12}) |
            Ui::vscroll() |
            Ui::grow());
}

Ui::Child appsList(State const &state) {
    return apps(
        iter(state.entries)
            .mapi(appRow)
            .collect<Ui::Children>());
}

Ui::Child appsPanel(State const &state) {
    return appsList(state) | panel();
}

Ui::Child appsFlyout(State const &state) {
    return Ui::vflow(
        runningApps(state),
        Ui::vflow(
            Ui::dragHandle(),
            appsList(state) | Ui::grow()) |
            Ui::box({
                .margin = {8, 8, 8, 8},
                .padding = {12, 0},
                .borderRadius = 8,
                .borderWidth = 1,
                .borderPaint = Ui::GRAY800,
                .backgroundPaint = Ui::GRAY950,
            }) |
            Ui::bound() |
            Ui::dismisable(
                Model::bind<Activate>(Panel::NIL),
                Ui::DismisDir::DOWN,
                0.3) |
            Ui::slideIn(Ui::SlideFrom::BOTTOM) | Ui::grow());
}

} // namespace Shell
