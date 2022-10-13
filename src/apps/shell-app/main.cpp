#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
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
    return Ui::button(
        std::move(onPress),
        Ui::ButtonStyle::subtle().withRadius(0),
        handle());
}

/* --- System Tray ---------------------------------------------------------- */

Ui::Child indicator(Media::Icons icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

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
    return handleButton(
        [](Ui::Node &n) {
            Ui::closeDialog(n);
        });
}

Ui::Child systemTray() {
    return Ui::vflow(
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
                        Ui::grow(notifications()))))),
        systemTrayHandle());
}

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child statusbar() {
    return Ui::button(
        [](Ui::Node &n) {
            Ui::showDialog(n, systemTray());
        },
        Ui::box(
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
                        indicator(Media::Icons::BATTERY))))));
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar() {
    return handleButton(
        [](Ui::Node &) {
        });
}

/* --- Home Screen ---------------------------------------------------------- */

Ui::Child homescreen() {
    return Ui::spacing(
        {48, 64},
        Ui::vflow(
            16,
            Ui::text(Ui::TextStyle::bold().withSize(48), "22:07"),
            Ui::text(Ui::TextStyle::bold().withSize(26), "Wed. 12 October")));
}

CliResult entryPoint(CliArgs args) {
    auto background = Ui::align(Layout::Align::COVER, Ui::image(try$(Media::loadImage("res/images/qoi-test/wikipedia_008.qoi"))));

    auto forground = Ui::vflow(
        statusbar(),
        Ui::grow(homescreen()),
        navbar());

    return Ui::runApp(
        args,
        Ui::dialogLayer(
            Ui::pinSize(
                {411, 731},
                Ui::stack(background, forground))));
}
