#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/react.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

namespace Shell {

/* --- Model ---------------------------------------------------------------- */

struct App {
    Mdi::Icon icon;
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

using Model = Ui::Model<State, Actions, reduce>;

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child systemTray();

Ui::Child indicator(Media::Icon icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

Ui::Child statusbar() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Gfx::BLACK,
        },
        Ui::minSize(
            {Ui::UNCONSTRAINED, 36},
            Ui::spacing(
                {12, 0},
                Ui::hflow(
                    4,
                    Ui::center(Ui::text(Ui::TextStyle::labelMedium(), "22:07")),
                    Ui::grow(NONE),
                    indicator(Mdi::NETWORK_STRENGTH_4),
                    indicator(Mdi::WIFI_STRENGTH_4),
                    indicator(Mdi::BATTERY)))));
}

Ui::Child statusbarButton() {
    return Ui::button(
        [](Ui::Node &n) {
            Ui::showDialog(n, systemTray());
        },
        statusbar());
}

/* --- System Tray ---------------------------------------------------------- */

Ui::Child quickSetting(Mdi::Icon icon) {
    return Ui::center(Ui::state<bool>(false, [icon](auto state) {
        return Ui::button(
            [state](Ui::Node &) mutable {
                state.update(not state.value());
            },
            (state.value()
                 ? Ui::ButtonStyle::primary().withForegroundPaint(Gfx::WHITE)
                 : Ui::ButtonStyle::secondary().withForegroundPaint(Gfx::ZINC300))
                .withRadius(99),
            Ui::minSize(48, Ui::center(Ui::icon(icon, 26))));
    }));
}

Ui::Child quickSettings() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Gfx::ZINC800,
        },
        Ui::spacing(
            8,
            Ui::hflow(
                12,
                quickSetting(Mdi::SWAP_VERTICAL),
                quickSetting(Mdi::WIFI_STRENGTH_4),
                quickSetting(Mdi::BLUETOOTH),
                quickSetting(Mdi::MAP_MARKER_OUTLINE),
                quickSetting(Mdi::FLASHLIGHT),
                Ui::grow(Ui::align(Layout::Align::END | Layout::Align::VFILL, quickSetting(Mdi::CHEVRON_DOWN))))));
}

Ui::Child notiWrapper(App app, Ui::Child inner) {
    return Ui::vflow(
        Ui::spacing(
            12,
            Ui::vflow(
                8,
                Ui::hflow(
                    4,
                    Ui::box(Ui::BoxStyle{.foregroundPaint = app.color[4]},
                            Ui::icon(app.icon, 12)),
                    Ui::text(Ui::TextStyle::labelMedium().withColor(Gfx::ZINC400), app.name)),
                inner)),
        Ui::separator());
}

Ui::Child notiMsg(String title, String body) {
    return Ui::vflow(
        6,
        Ui::hflow(Ui::text(Ui::TextStyle::labelLarge(), title)),
        Ui::text(Ui::TextStyle::labelMedium(), body));
}

Ui::Child notification(Mdi::Icon icon, String title, String subtitle) {
    return notiWrapper({icon, Gfx::BLUE_RAMP, "Hello, world"}, notiMsg(title, subtitle));
}

Ui::Child notifications() {
    return Ui::vflow(
        Ui::spacing(
            {12, 0},
            Ui::hflow(

                Ui::center(Ui::text(Ui::TextStyle::labelMedium(), "Notifications")),
                Ui::grow(NONE),
                Ui::button(
                    [](Ui::Node &) {
                    },
                    Ui::ButtonStyle::subtle(),
                    "Clear All"))),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"));
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
                            .borderRadius = {0, 0, 8, 8},
                            .backgroundPaint = Gfx::ZINC900,
                        },
                        Ui::vflow(8, quickSettings(), notifications() | Ui::grow(), Ui::dragHandle())))),
        Ui::empty(16));
}

/* --- Application Drawer --------------------------------------------------- */

Ui::Child searchInput() {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = 4,
            .borderWidth = 1,
            .backgroundPaint = Gfx::ZINC800,
        },
        Ui::minSize({Ui::UNCONSTRAINED, 48},
                    Ui::spacing(
                        {12, 8},
                        Ui::hflow(
                            Ui::grow(Ui::vcenter(Ui::text(Ui::TextStyle::labelMedium(), "Search..."))),
                            Ui::center(Ui::icon(Mdi::MAGNIFY, 24))))));
}

Ui::Child appIcon(Mdi::Icon icon, Gfx::ColorRamp colors) {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = 99,
            .backgroundPaint = colors[3],
            .foregroundPaint = colors[6],
        },
        Ui::center(
            Ui::spacing(8,
                        Ui::icon(icon, 22))));
}

Ui::Child appRow(Mdi::Icon icon, Gfx::ColorRamp colors, String title) {
    return Ui::spacing(
        {12, 8, 12, 8},
        Ui::hflow(
            12,
            appIcon(icon, colors),
            Ui::center(Ui::text(Ui::TextStyle::labelMedium(), title))));
}

Ui::Child apps(Ui::Children apps) {
    return Ui::vflow(
        searchInput(),
        Ui::vflow(apps) |
            Ui::spacing({0, 12}) |
            Ui::vscroll() |
            Ui::grow());
}

Ui::Child appDrawer() {
    Ui::Children appItems = {
        appRow(Mdi::CALCULATOR, Gfx::ORANGE_RAMP, "Calculator"),
        appRow(Mdi::CALENDAR, Gfx::PURPLE_RAMP, "Calendar"),
        appRow(Mdi::CAMERA, Gfx::TEAL_RAMP, "Camera"),
        appRow(Mdi::CLOCK, Gfx::RED_RAMP, "Clock"),
        appRow(Mdi::COG, Gfx::LIME_RAMP, "Settings"),
        appRow(Mdi::EMAIL, Gfx::BLUE_RAMP, "Email"),
        appRow(Mdi::FILE, Gfx::ORANGE_RAMP, "Files"),
        appRow(Mdi::FORMAT_FONT, Gfx::YELLOW_RAMP, "Text Editor"),
        appRow(Mdi::WEB, Gfx::LIME_RAMP, "Browser"),
        appRow(Mdi::HAND_WAVE, Gfx::BLUE_RAMP, "Hello World"),
        appRow(Mdi::MAP_MARKER_OUTLINE, Gfx::PINK_RAMP, "Maps"),
        appRow(Mdi::MUSIC, Gfx::YELLOW_RAMP, "Music"),
        appRow(Mdi::PHONE, Gfx::BLUE_RAMP, "Phone"),
    };

    return Ui::vflow(
        // statusbar(),
        Ui::empty(64),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::DOWN,
                0.3,
                Ui::box(
                    Ui::BoxStyle{
                        .borderRadius = 8,
                        .backgroundPaint = Gfx::ZINC900,
                    },
                    Ui::vflow(
                        Ui::dragHandle(),
                        Ui::grow(
                            Ui::spacing(
                                {12, 0},
                                apps(appItems))))) |
                    Ui::spacing(8))));
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
        Ui::grow(NONE),
        navbar());
}

/* --- Lockscreen ----------------------------------------------------------- */

Ui::Child lockscreen() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Gfx::Gradient::vlinear()
                                   .withColors(Gfx::BLACK.withOpacity(0.5),
                                               Gfx::BLACK.withOpacity(0.75))
                                   .bake(),
        },

        Ui::dismisable(
            Model::bind<UnlockAction>(),
            Ui::DismisDir::TOP,
            0.3,
            Ui::dragRegion(
                Ui::spacing(
                    {48, 64},
                    Ui::vflow(
                        Ui::center(Ui::text(Ui::TextStyle::displayMedium(), "22:07")),
                        Ui::empty(16),
                        Ui::center(Ui::text(Ui::TextStyle::titleMedium(), "Wed. 12 October")),
                        Ui::grow(NONE),
                        Ui::vflow(
                            Ui::center(Ui::icon(Mdi::CHEVRON_UP, 48)),
                            Ui::center(Ui::text(Ui::TextStyle::labelLarge(), "Swipe up to unlock"))))))));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](auto state) {
        auto wallpapers = Media::loadImageOrFallback("bundle://skift-wallpapers/images/brutal.qoi"_url).unwrap();
        auto background = Ui::align(Layout::Align::COVER, Ui::image(wallpapers));
        return Ui::dialogLayer(
            Ui::pinSize(
                {411, 731},
                Ui::stack(background, state.locked ? lockscreen() : homeScreen())));
    });
}

} // namespace Shell

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Shell::app());
}
