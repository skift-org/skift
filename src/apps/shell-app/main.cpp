#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

/* --- Common --------------------------------------------------------------- */

Ui::Child handle() {
    return Ui::minSize(
        {Ui::UNCONSTRAINED, 48},
        Ui::center(
            Ui::box(
                Ui::BoxStyle{
                    .borderRadius = 999,
                    .backgroundColor = Gfx::WHITE,
                },
                Ui::empty({128, 8}))));
}

Ui::Child handleButton(Ui::OnPress onPress) {
    return Ui::minSize(
        {Ui::UNCONSTRAINED, 48},
        Ui::center(
            Ui::button(
                std::move(onPress),
                Ui::ButtonStyle::subtle().withRadius(99),
                Ui::spacing(12,
                            Ui::box(
                                Ui::BoxStyle{
                                    .borderRadius = 999,
                                    .backgroundColor = Gfx::WHITE,
                                },
                                Ui::empty({128, 8}))))));
}

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child systemTray();

Ui::Child indicator(Media::Icons icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

Ui::Child statusbar() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundColor = Gfx::BLACK,
        },
        Ui::minSize(
            {Ui::UNCONSTRAINED, 36},
            Ui::spacing(
                {12, 0},
                Ui::hflow(
                    4,
                    Ui::center(Ui::text(Ui::TextStyle::bold().withSize(14), "22:07")),
                    Ui::grow(),
                    indicator(Media::Icons::NETWORK_STRENGTH_4),
                    indicator(Media::Icons::WIFI_STRENGTH_4),
                    indicator(Media::Icons::BATTERY)))));
}

Ui::Child statusbarButton() {
    return Ui::button(
        [](Ui::Node &n) {
            Ui::showDialog(n, systemTray());
        },
        statusbar());
}

/* --- System Tray ---------------------------------------------------------- */

Ui::Child quickSetting(Media::Icons icon) {
    return Ui::center(
        Ui::button(
            [](Ui::Node &) {
            },
            Ui::ButtonStyle::subtle().withRadius(99),
            Ui::minSize(48, Ui::center(Ui::icon(icon, 26)))));
}

Ui::Child quickSettings() {
    return Ui::card(
        Ui::spacing(
            8,
            Ui::hflow(
                12,
                quickSetting(Media::Icons::SWAP_VERTICAL),
                quickSetting(Media::Icons::WIFI_STRENGTH_4),
                quickSetting(Media::Icons::BLUETOOTH),
                quickSetting(Media::Icons::MAP_MARKER_OUTLINE),
                quickSetting(Media::Icons::FLASHLIGHT),
                Ui::grow(Ui::align(Layout::Align::END | Layout::Align::VFILL, quickSetting(Media::Icons::CHEVRON_DOWN))))));
}

Ui::Child notification(Media::Icons icon, String title, String subtitle) {
    return Ui::card(
        Ui::spacing(
            {12, 8, 12, 8},
            Ui::vflow(
                8,
                Ui::hflow(
                    8,
                    Ui::center(Ui::icon(icon)),
                    Ui::vflow(
                        4,
                        Ui::text(Ui::TextStyle::bold(), title),
                        Ui::text(Ui::TextStyle::body(), subtitle))))));
}

Ui::Child notifications() {
    return Ui::vflow(
        8,
        Ui::hflow(
            Ui::spacing(
                {12, 0},
                Ui::center(Ui::text(Ui::TextStyle::label(), "Notifications"))),
            Ui::grow(),
            Ui::button(
                [](Ui::Node &) {
                },
                Ui::ButtonStyle::subtle(),
                "Clear All")),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"));
}

Ui::Child systemTrayHandle() {
    return handleButton(Ui::closeDialog);
}

Ui::Child systemTray() {
    return Ui::vflow(
        statusbar(),
        Ui::grow(
            Ui::box(
                Ui::BoxStyle{
                    .borderRadius = {0, 0, 16, 16},
                    .backgroundColor = Gfx::ZINC900,
                },
                Ui::spacing(
                    {12, 8, 12, 0},
                    Ui::vflow(
                        8,
                        quickSettings(),
                        Ui::grow(notifications()),
                        handleButton(Ui::closeDialog))))),
        Ui::empty(16));
}

/* --- Application Drawer --------------------------------------------------- */

Ui::Child searchInput() {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = 4,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC700,
            .backgroundColor = Gfx::ZINC800,
        },
        Ui::minSize({Ui::UNCONSTRAINED, 48},
                    Ui::spacing(
                        {12, 8},
                        Ui::hflow(
                            Ui::grow(Ui::vcenter(Ui::text(Ui::TextStyle::label().withSize(18), "Search"))),
                            Ui::center(Ui::icon(Media::Icons::MAGNIFY, 24))))));
}

Ui::Child appIcon(Media::Icons icon, Gfx::ColorRamp colors) {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = 99,
            .backgroundColor = colors[3],
            .foregroundColor = colors[6],
        },
        Ui::center(
            Ui::spacing(12,
                        Ui::icon(icon, 18))));
}

Ui::Child appRow(Media::Icons icon, Gfx::ColorRamp colors, String title) {
    return Ui::spacing(
        {12, 8, 12, 8},
        Ui::hflow(
            12,
            appIcon(icon, colors),
            Ui::center(Ui::text(Ui::TextStyle::label().withSize(18), title))));
}

Ui::Child applications() {
    return Ui::grow(
        Ui::vflow(
            searchInput(),
            Ui::grow(
                Ui::vscroll(
                    Ui::spacing(
                        {0, 12},
                        Ui::vflow(
                            appRow(Media::Icons::CALCULATOR, Gfx::ORANGE_RAMP, "Calculator"),
                            appRow(Media::Icons::CALENDAR, Gfx::PURPLE_RAMP, "Calendar"),
                            appRow(Media::Icons::CAMERA, Gfx::TEAL_RAMP, "Camera"),
                            appRow(Media::Icons::CLOCK, Gfx::RED_RAMP, "Clock"),
                            appRow(Media::Icons::COG, Gfx::LIME_RAMP, "Settings"),
                            appRow(Media::Icons::EMAIL, Gfx::BLUE_RAMP, "Email"),
                            appRow(Media::Icons::FACEBOOK, Gfx::PURPLE_RAMP, "Facebook"),
                            appRow(Media::Icons::FILE, Gfx::ORANGE_RAMP, "Files"),
                            appRow(Media::Icons::FORMAT_FONT, Gfx::YELLOW_RAMP, "Text Editor"),
                            appRow(Media::Icons::GLOBE_LIGHT, Gfx::LIME_RAMP, "Browser"),
                            appRow(Media::Icons::HAND_WAVE, Gfx::BLUE_RAMP, "Hello World"),
                            appRow(Media::Icons::INSTAGRAM, Gfx::YELLOW_RAMP, "Instagram"),
                            appRow(Media::Icons::MAP_MARKER_OUTLINE, Gfx::PINK_RAMP, "Maps"),
                            appRow(Media::Icons::MUSIC, Gfx::YELLOW_RAMP, "Music"),
                            appRow(Media::Icons::PHONE, Gfx::BLUE_RAMP, "Phone"),
                            appRow(Media::Icons::TWITTER, Gfx::RED_RAMP, "Twitter"),
                            appRow(Media::Icons::YOUTUBE, Gfx::PINK_RAMP, "YouTube")))))));
}

Ui::Child appDrawer() {
    return Ui::vflow(
        statusbar(),
        Ui::empty(24),
        Ui::grow(
            Ui::box(
                Ui::BoxStyle{
                    .borderRadius = {16, 16, 16, 16},
                    .backgroundColor = Gfx::ZINC900,
                },
                Ui::vflow(
                    handleButton(Ui::closeDialog),
                    Ui::grow(Ui::spacing({12, 0}, applications()))))));
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar() {
    return handleButton(
        [](Ui::Node &n) {
            Ui::showDialog(n, appDrawer());
        });
}

/* --- Home Screen ---------------------------------------------------------- */

Ui::Child homescreen() {
    return Ui::spacing(
        {48, 64},
        Ui::vflow(
            16,
            Ui::text(Ui::TextStyle::bold().withSize(48).withColor(Gfx::ZINC900), "22:07"),
            Ui::text(Ui::TextStyle::bold().withSize(26).withColor(Gfx::ZINC900), "Wed. 12 October")));
}

CliResult entryPoint(CliArgs args) {
    auto background = Ui::align(Layout::Align::COVER, Ui::image(try$(Media::loadImage("res/images/wallpapers/nys-museum.qoi"))));

    auto forground = Ui::vflow(
        statusbarButton(),
        Ui::grow(homescreen()),
        navbar());

    return Ui::runApp(
        args,
        Ui::dialogLayer(
            Ui::pinSize(
                {411, 731},
                Ui::stack(background, forground))));
}
