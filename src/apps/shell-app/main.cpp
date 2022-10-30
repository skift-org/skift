#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

/* --- Model ---------------------------------------------------------------- */

struct App {
    Media::Icons icon;
    Gfx::ColorRamp color;
    String name;
};

struct Noti {
    String title;
    String msg;
};

struct State {
    bool locked = true;
};

struct UnlockAction {};

using Actions = Var<UnlockAction>;

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](UnlockAction) {
            state.locked = false;
            return state;
        }});
}

using Model = Ui::Model<State, Actions>;

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
    return Ui::center(Ui::state<bool>(false, [icon](auto state) {
        return Ui::button(
            [state](Ui::Node &) mutable {
                state.update(!state.value());
            },
            (state.value()
                 ? Ui::ButtonStyle::primary().withForegroundColor(Gfx::WHITE)
                 : Ui::ButtonStyle::secondary().withForegroundColor(Gfx::ZINC300))
                .withRadius(99),
            Ui::minSize(48, Ui::center(Ui::icon(icon, 26))));
    }));
}

Ui::Child quickSettings() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundColor = Gfx::ZINC800,
        },
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

Ui::Child notiWrapper(App app, Ui::Child inner) {
    return Ui::vflow(
        Ui::spacing(
            12,
            Ui::vflow(
                8,
                Ui::hflow(
                    4,
                    Ui::box(Ui::BoxStyle{.foregroundColor = app.color[4]},
                            Ui::icon(app.icon, 12)),
                    Ui::text(Ui::TextStyle::regular().withColor(Gfx::ZINC400), app.name)),
                inner)),
        Ui::separator());
}

Ui::Child notiMsg(String title, String body) {
    return Ui::vflow(
        6,
        Ui::hflow(Ui::text(Ui::TextStyle::bold(), title)),
        Ui::text(Ui::TextStyle::regular(), body));
}

Ui::Child notification(Media::Icons icon, String title, String subtitle) {
    return notiWrapper({icon, Gfx::BLUE_RAMP, "Hello, world"}, notiMsg(title, subtitle));
}

Ui::Child notifications() {
    return Ui::vflow(
        Ui::spacing(
            {12, 0},
            Ui::hflow(

                Ui::center(Ui::text(Ui::TextStyle::label(), "Notifications")),
                Ui::grow(),
                Ui::button(
                    [](Ui::Node &) {
                    },
                    Ui::ButtonStyle::subtle(),
                    "Clear All"))),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Media::Icons::HAND_WAVE, "Hello", "Hello, world!"));
}

Ui::Child systemTray() {
    return Ui::vflow(
        statusbar(),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::TOP,
                0.3,
                Ui::box(Ui::BoxStyle{
                            .borderRadius = {0, 0, 16, 16},
                            .backgroundColor = Gfx::ZINC900,
                        },

                        Ui::vflow(8, quickSettings(), Ui::grow(notifications()), Ui::dragHandle())))),
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
                            Ui::grow(Ui::vcenter(Ui::text(Ui::TextStyle::label().withSize(18), "Search..."))),
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
            Ui::spacing(8,
                        Ui::icon(icon, 22))));
}

Ui::Child appRow(Media::Icons icon, Gfx::ColorRamp colors, String title) {
    return Ui::spacing(
        {12, 8, 12, 8},
        Ui::hflow(
            12,
            appIcon(icon, colors),
            Ui::center(Ui::text(Ui::TextStyle::label().withSize(16), title))));
}

Ui::Child apps(Ui::Children apps) {
    return Ui::grow(
        Ui::vflow(
            searchInput(),
            Ui::grow(
                Ui::vscroll(
                    Ui::spacing(
                        {0, 12},
                        Ui::vflow(
                            apps))))));
}

Ui::Child appDrawer() {
    Ui::Children appItems = {
        appRow(Media::Icons::CALCULATOR, Gfx::ORANGE_RAMP, "Calculator"),
        appRow(Media::Icons::CALENDAR, Gfx::PURPLE_RAMP, "Calendar"),
        appRow(Media::Icons::CAMERA, Gfx::TEAL_RAMP, "Camera"),
        appRow(Media::Icons::CLOCK, Gfx::RED_RAMP, "Clock"),
        appRow(Media::Icons::COG, Gfx::LIME_RAMP, "Settings"),
        appRow(Media::Icons::EMAIL, Gfx::BLUE_RAMP, "Email"),
        appRow(Media::Icons::FILE, Gfx::ORANGE_RAMP, "Files"),
        appRow(Media::Icons::FORMAT_FONT, Gfx::YELLOW_RAMP, "Text Editor"),
        appRow(Media::Icons::WEB, Gfx::LIME_RAMP, "Browser"),
        appRow(Media::Icons::HAND_WAVE, Gfx::BLUE_RAMP, "Hello World"),
        appRow(Media::Icons::MAP_MARKER_OUTLINE, Gfx::PINK_RAMP, "Maps"),
        appRow(Media::Icons::MUSIC, Gfx::YELLOW_RAMP, "Music"),
        appRow(Media::Icons::PHONE, Gfx::BLUE_RAMP, "Phone"),
    };

    return Ui::vflow(
        statusbar(),
        Ui::empty(24),
        Ui::grow(
            Ui::dismisable(Ui::closeDialog, Ui::DismisDir::DOWN, 0.3,
                           Ui::box(
                               Ui::BoxStyle{
                                   .borderRadius = {16, 16, 16, 16},
                                   .backgroundColor = Gfx::ZINC900,
                               },
                               Ui::vflow(
                                   Ui::dragHandle(),
                                   Ui::grow(Ui::spacing({12, 0}, apps(appItems))))))));
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar() {
    return Ui::buttonHandle([](Ui::Node &n) {
        Ui::showDialog(n, appDrawer());
    });
}

/* --- Home Screen ---------------------------------------------------------- */

Ui::Child homeScreen() {
    return Ui::vflow(
        statusbarButton(),
        Ui::grow(),
        navbar());
}

/* --- Lockscreen ----------------------------------------------------------- */

Ui::Child lockscreen() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundColor = Gfx::BLACK.withOpacity(0.5),
        },
        Ui::vflow(
            Ui::grow(
                Ui::dismisable(
                    Model::bind<UnlockAction>(),
                    Ui::DismisDir::TOP,
                    0.3,
                    Ui::dragRegion(
                        Ui::spacing(
                            {48, 64},
                            Ui::vflow(
                                Ui::center(Ui::text(Ui::TextStyle::regular().withSize(52), "22:07")),
                                Ui::empty(16),
                                Ui::center(Ui::text(Ui::TextStyle::bold().withSize(16), "Wed. 12 October")),
                                Ui::grow(),
                                Ui::vflow(
                                    Ui::center(Ui::icon(Media::Icons::CHEVRON_UP, 48)),
                                    Ui::center(Ui::text(Ui::TextStyle::regular().withSize(16), "Swipe up to unlock"))))))))));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, reduce, [](auto state) {
        auto background = Ui::align(Layout::Align::COVER, Ui::image(Media::loadImage("res/images/wallpapers/nys-museum.qoi").unwrap()));
        return Ui::dialogLayer(
            Ui::pinSize(
                {411, 731},
                Ui::stack(background, state.locked ? lockscreen() : homeScreen())));
    });
}

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, app());
}
