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
           Ui::button([](auto &n) {
               Ui::showDialog(n, keyboardFlyout());
           });
}

Ui::Child appIcon(Mdi::Icon icon, Gfx::ColorRamp colors, isize size = 22) {
    return Ui::icon(icon, size) |
           Ui::spacing(size / 2.75) |
           Ui::center() |
           Ui::box({
               .borderRadius = size * 0.25,
               .borderWidth = 1,
               .borderPaint = colors[5],
               .backgroundPaint = colors[6],
               .foregroundPaint = colors[1],
           });
}

Ui::Child appRow(Mdi::Icon icon, Gfx::ColorRamp colors, String title) {
    return Ui::button(
        Ui::NOP,
        Ui::ButtonStyle::subtle(),
        Ui::hflow(
            12,
            Layout::Align::START | Layout::Align::VCENTER,
            appIcon(icon, colors),
            Ui::labelLarge(title)) |
            Ui::spacing(6));
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

Ui::Child appsList() {
    Ui::Children appItems = {
        appRow(Mdi::INFORMATION_OUTLINE, Gfx::BLUE_RAMP, "About"),
        appRow(Mdi::CALCULATOR, Gfx::ORANGE_RAMP, "Calculator"),
        appRow(Mdi::PALETTE_SWATCH, Gfx::RED_RAMP, "Color Picker"),
        appRow(Mdi::COUNTER, Gfx::GREEN_RAMP, "Counter"),
        appRow(Mdi::DUCK, Gfx::YELLOW_RAMP, "Demos"),
        appRow(Mdi::FILE, Gfx::ORANGE_RAMP, "Files"),
        appRow(Mdi::FORMAT_FONT, Gfx::BLUE_RAMP, "Fonts"),
        appRow(Mdi::EMOTICON, Gfx::RED_RAMP, "Hello World"),
        appRow(Mdi::IMAGE, Gfx::GREEN_RAMP, "Icons"),
        appRow(Mdi::IMAGE, Gfx::YELLOW_RAMP, "Image Viewer"),
        appRow(Mdi::CUBE, Gfx::BLUE_RAMP, "Ray Tracer"),
        appRow(Mdi::COG, Gfx::ZINC_RAMP, "Settings"),
        appRow(Mdi::TABLE, Gfx::GREEN_RAMP, "Spreadsheet"),
        appRow(Mdi::WIDGETS, Gfx::BLUE_RAMP, "Widget Gallery"),
    };

    return apps(appItems);
}

Ui::Child appsPanel() {
    return appsList() | panel();
}

Ui::Child appsFlyout() {
    return Ui::vflow(
               Ui::dragHandle(),
               appsList() | Ui::grow()) |
           Ui::box({
               .margin = {8, 8 + 64, 8, 8},
               .padding = {12, 0},
               .borderRadius = 8,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY800,
               .backgroundPaint = Ui::GRAY950,
           }) |
           Ui::bound() |
           Ui::dismisable(
               Ui::closeDialog,
               Ui::DismisDir::DOWN,
               0.3);
}

} // namespace Shell
