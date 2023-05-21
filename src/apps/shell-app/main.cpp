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
    bool isTablet = true;
    bool locked = true;
};

struct ToggleTablet {};

struct Unlock {};

using Actions = Var<ToggleTablet, Unlock>;

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](ToggleTablet) {
            debug("toggle tablet");
            state.isTablet = not state.isTablet;
            return state;
        },
        [&](Unlock) {
            state.locked = false;
            return state;
        }});
}

using Model = Ui::Model<State, Actions, reduce>;

/* --- Status Bar ----------------------------------------------------------- */

Ui::Child systemTray(State const &);

Ui::Child indicator(Media::Icon icon) {
    return Ui::spacing({4}, Ui::center(Ui::icon(icon)));
}

Ui::Child statusbar() {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Gfx::ZINC900,
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

Ui::Child statusbarButton(State const &state) {
    return Ui::button(
        [&](Ui::Node &n) {
            Ui::showDialog(n, systemTray(state));
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
                 : Ui::ButtonStyle::secondary().withForegroundPaint(Ui::GRAY300))
                .withRadius(99),
            Ui::minSize(48, Ui::center(Ui::icon(icon, 26))));
    }));
}

Ui::Child quickSettings(State const &state) {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Ui::GRAY900,
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
                Ui::button(Model::bind<ToggleTablet>(), Ui::ButtonStyle::secondary(), state.isTablet ? Mdi::CELLPHONE : Mdi::TABLET),
                quickSetting(Mdi::CHEVRON_DOWN) |
                    Ui::end() |
                    Ui::grow())));
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
                    Ui::text(Ui::TextStyle::labelMedium().withColor(Ui::GRAY400), app.name)),
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
                    Ui::NOP,
                    Ui::ButtonStyle::subtle(),
                    "Clear All"))),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"));
}

Ui::Child systemTray(State const &state) {
    return Ui::vflow(
        statusbar(),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::TOP,
                0.3,
                Ui::box(Ui::BoxStyle{
                            .borderRadius = {0, 0, 8, 8},
                            .backgroundPaint = Ui::GRAY950,
                        },
                        Ui::vflow(quickSettings(state), Ui::separator(), notifications() | Ui::grow(), Ui::dragHandle())))),
        Ui::empty(16));
}

/* --- Application Drawer --------------------------------------------------- */

Ui::Child searchInput() {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = 4,
            .borderWidth = 1,
            .backgroundPaint = Ui::GRAY900,
        },
        Ui::minSize({Ui::UNCONSTRAINED, 48},
                    Ui::spacing(
                        {12, 8},
                        Ui::hflow(
                            Ui::grow(Ui::vcenter(Ui::text(Ui::TextStyle::labelMedium(), "Search..."))),
                            Ui::center(Ui::icon(Mdi::MAGNIFY, 24))))));
}

Ui::Child appIcon(Mdi::Icon icon, Gfx::ColorRamp colors, isize size = 22) {
    return Ui::box(
        Ui::BoxStyle{
            .borderRadius = size * 0.25,
            .borderWidth = 1,
            .borderPaint = colors[6],
            .backgroundPaint = Gfx::Gradient::linear().withColors(colors[6], colors[7]).withStart({0, 0}).withEnd({0, 1}).bake(),
            .foregroundPaint = colors[1],
        },
        Ui::center(
            Ui::spacing(8,
                        Ui::icon(icon, size))));
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

Ui::Child appsDrawer() {
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

    return Ui::box(
               Ui::BoxStyle{
                   .borderRadius = 8,
                   .borderWidth = 1,
                   .borderPaint = Ui::GRAY800,
                   .backgroundPaint = Ui::GRAY950,
               },
               Ui::vflow(
                   Ui::dragHandle(),
                   Ui::grow(
                       Ui::spacing(
                           {12, 0},
                           apps(appItems))))) |
           Ui::spacing(8);
}

Ui::Child appsDialog() {
    return Ui::vflow(
        Ui::empty(64),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::DOWN,
                0.3,
                appsDrawer())));
}

Ui::Child appsFlyout() {
    return Ui::vflow(
        Ui::empty(64),
        Ui::grow(
            Ui::dismisable(
                Ui::closeDialog,
                Ui::DismisDir::DOWN,
                0.3,
                appsDrawer() | Ui::pinSize({500, 500}) | Ui::align(Layout::Align::BOTTOM | Layout::Align::START))));
}

/* --- Navigation Bar ------------------------------------------------------- */

Ui::Child navbar() {
    return Ui::buttonHandle([](Ui::Node &n) {
        Ui::showDialog(n, appsDialog());
    });
}

/* --- Lockscreen ----------------------------------------------------------- */

Ui::Child lockscreen(State const &state) {
    return Ui::box(
        Ui::BoxStyle{
            .backgroundPaint = Gfx::Gradient::vlinear()
                                   .withColors(Gfx::BLACK.withOpacity(0.5),
                                               Gfx::BLACK.withOpacity(0.75))
                                   .bake(),
        },

        Ui::dismisable(
            Model::bind<Unlock>(),
            Ui::DismisDir::TOP,
            0.3,
            Ui::dragRegion(
                Ui::spacing(
                    state.isTablet ? 64 : 128,
                    Ui::vflow(
                        Ui::center(Ui::text(Ui::TextStyle::displayMedium(), "22:07")),
                        Ui::empty(16),
                        Ui::center(Ui::text(Ui::TextStyle::titleMedium(), "Wed. 12 October")),
                        Ui::grow(NONE),
                        Ui::vflow(
                            Ui::center(Ui::icon(Mdi::CHEVRON_UP, 48)),
                            Ui::center(Ui::text(
                                Ui::TextStyle::labelLarge(),
                                state.isTablet ? "Swipe up to unlock"
                                               : "Swipe up or press any key to unlock"))))))));
}

/* --- Taskbar -------------------------------------------------------------- */

Ui::Child taskbar(State const &state) {
    auto appsButton = Ui::button(
        [](Ui::Node &n) {
            Ui::showDialog(n, appsFlyout());
        },
        Ui::ButtonStyle::subtle(),
        Mdi::APPS, "Applications");

    auto calButton = Ui::button(
        Ui::NOP,
        Ui::ButtonStyle::subtle(),
        Mdi::CALENDAR, "Jan. 12 2021, 22:07");

    auto trayButton = Ui::button(
        [&](Ui::Node &n) {
            Ui::showDialog(n, systemTray(state));
        },
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
               trayButton) |
           Ui::box(Ui::BoxStyle{
               .padding = 6,
               .backgroundPaint = Ui::GRAY950.withOpacity(0.95),
           });
}

/* --- Shells --------------------------------------------------------------- */

Ui::Child tablet(State const &state) {
    return Ui::vflow(
        statusbarButton(state),
        Ui::grow(NONE),
        navbar());
}

Ui::Child desktop(State const &state) {
    return Ui::vflow(taskbar(state), Ui::separator(), Ui::grow(NONE));
}

Ui::Child app(bool isTablet) {
    return Ui::reducer<Model>({.isTablet = isTablet}, [](auto state) {
        auto wallpapers = Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap();
        auto background = Ui::align(Layout::Align::COVER, Ui::image(wallpapers));

        return Ui::dialogLayer(
            Ui::pinSize(
                state.isTablet ? Math::Vec2i{411, 731} : Math::Vec2i{1280, 1024},
                Ui::stack(
                    background,
                    state.locked ? lockscreen(state)
                                 : (state.isTablet ? tablet(state)
                                                   : desktop(state)))));
    });
}

} // namespace Shell

Res<> entryPoint(Ctx &ctx) {
    auto args = useArgs(ctx);
    bool isTablet = args.has("+tablet");
    return Ui::runApp(ctx, Shell::app(isTablet));
}
