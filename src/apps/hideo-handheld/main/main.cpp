#include <karm-gfx/colors.h>
#include <karm-gfx/font.h>
#include <karm-gfx/icon.h>
#include <karm-gfx/prose.h>
#include <karm-gfx/shadow.h>
#include <karm-math/align.h>
#include <karm-sys/entry.h>

import Karm.App;
import Karm.Kira;
import Karm.Ui;
import Karm.Image;
import Karm.Font;
import Mdi;

using namespace Karm;

namespace Hideo::Handheld {

struct State {
    bool quickMenuVisible;

    struct QuickMenuToggle {};

    using Action = Union<QuickMenuToggle>;

    Ui::Task<Action> reduce(Action const& action) {
        return action.visit(Visitor{
            [&](QuickMenuToggle) {
                quickMenuVisible = not quickMenuVisible;
                return NONE;
            },
        });
    }
};

using Model = Ui::Model<State, State::Action>;

static Opt<Rc<Gfx::Fontface>> _inputFontface = NONE;

Rc<Gfx::Fontface> inputFontface() {
    if (not _inputFontface) {
        _inputFontface = Font::loadFontfaceOrFallback("bundle://hideo-handheld/fonts/BPreplayBold.ttf"_url).unwrap();
    }
    return *_inputFontface;
}

Gfx::ProseStyle inputMedium() {
    return {
        .font = Gfx::Font{
            inputFontface(),
            16,
        },
    };
}

Ui::Child logo(Str text) {
    return Ui::titleMedium(text) | Ui::vcenter();
}

Ui::Child buttonHint(Str button, Str description, Gfx::Color color) {
    return Ui::hflow(
        Ui::text(inputMedium(), button) | Ui::center() | Ui::minSize(26) |
            Ui::box({
                .margin = 4,
                .borderRadii = 999,
                .backgroundFill = color,
                .foregroundFill = color.luminance() > 0.6 ? Gfx::BLACK : Gfx::WHITE,
            }),
        Ui::labelLarge(description) | Ui::center() | Ui::insets({0, 12, 0, 2})
    );
}

Ui::Child statusWidget(Gfx::Icon icon) {
    return Ui::hflow(
               12,
               Ui::icon(icon) | Ui::center() | Ui::minSize(22)
           ) |
           Ui::box({
               .padding = 8,
               .borderRadii = 999,
               .borderWidth = 1,
               .borderFill = Ui::GRAY700,
               .backgroundFill = Ui::GRAY800,
           });
}

Ui::Child statusWidget(Str description) {
    return Ui::labelLarge(description) | Ui::center() |
           Ui::box({
               .padding = {8, 16},
               .borderRadii = 999,
               .borderWidth = 1,
               .borderFill = Ui::GRAY700,
               .backgroundFill = Ui::GRAY800,
           });
}

Ui::Child statusWidget(Gfx::Icon icon, Str description) {
    return Ui::hflow(
               6,
               Ui::icon(icon) | Ui::center() | Ui::minSize(22), Ui::labelLarge(description) | Ui::center()
           ) |
           Ui::box({
               .padding = {8, 16, 8, 8},
               .borderRadii = 999,
               .borderWidth = 1,
               .borderFill = Ui::GRAY700,
               .backgroundFill = Ui::GRAY800,
           });
}

Ui::Child tileGameCover(Ref::Url image) {
    return Ui::image(image, 6);
}

Ui::Child tileAppCover(Gfx::Icon icon, Gfx::ColorRamp ramp) {
    return Ui::icon(icon, 96) |
           Ui::center() | Ui::bound() |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = ramp[6],
               .foregroundFill = ramp[1],
           });
}

Ui::Child tileContent(Ui::Child child) {
    return child |
           Ui::pinSize(192) |
           Ui::box({
               .borderRadii = 6,
               .shadowStyle = Gfx::BoxShadow::elevated(8),
           }) |
           Ui::align(Math::Align::BOTTOM | Math::Align::START);
}

Ui::Child tileButton(Ui::Send<> onPress, Ui::Child child) {
    return Ui::button(
        onPress,
        Ui::ButtonStyle{
            .hoverStyle = {
                .borderRadii = 6,
                .borderWidth = 2,
                .borderFill = Ui::ACCENT500,
            },
            .pressStyle = {
                .borderRadii = 6,
                .borderWidth = 2,
                .borderFill = Ui::ACCENT400,
            },
        },
        child
    );
}

Ui::Child appItem() {
    return tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/celeste.qoi"_url)));
}

Ui::Child appList() {
    return Ui::vflow(
               Ui::grow(NONE),
               Ui::headlineMedium("Celeste") |
                   Ui::insets({0, 48, 16}),
               Ui::hflow(
                   26,
                   tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/celeste.qoi"_url))),
                   tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/doom.qoi"_url))),
                   tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/minicraft.qoi"_url))),
                   tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/vvvvvv.qoi"_url))),
                   tileButton(Ui::SINK<>, tileContent(tileGameCover("bundle://hideo-handheld/tiles/lego-island.qoi"_url))),
                   tileButton(Ui::SINK<>, tileContent(tileAppCover(Mdi::FOLDER, Gfx::EMERALD_RAMP))),

                   Kr::separator(Gfx::GRAY500),
                   tileButton(Ui::SINK<>, tileContent(tileAppCover(Mdi::APPS, Gfx::ZINC_RAMP)))
               ) | Ui::insets({0, 48, 48}) |
                   Ui::hscroll()
           ) |
           Ui::grow();
}

Ui::Child quickSettings() {
    return Ui::hflow(
               Ui::grow(NONE),
               Ui::hflow(
                   Kr::separator(),
                   Ui::vflow(
                       Kr::titleRow("Sound"s),
                       Kr::sliderRow(0.5, Ui::SINK<f64>, "Volume"s),

                       Kr::separator(),
                       Kr::titleRow("Display"s),
                       Kr::sliderRow(0.5, Ui::SINK<f64>, "Brightness"s),
                       Kr::toggleRow(true, Ui::SINK<bool>, "Night Mode"s),

                       Kr::separator(),
                       Kr::titleRow("Wireless"s),
                       Kr::toggleRow(true, Ui::SINK<bool>, "NFC"s),
                       Kr::toggleRow(true, Ui::SINK<bool>, "RaftShare™"s),
                       Kr::rowContent(Ui::text(Ui::TextStyles::bodyMedium().withColor(Ui::GRAY400), "RaftShare™ keeps consoles together like otters in a raft, letting you share games and apps with the ones around you, wirelessly.")),

                       Kr::separator(),
                       Kr::titleRow("Appearance"s),
                       Kr::toggleRow(true, Ui::SINK<bool>, "Dark Mode"s),

                       Kr::separator(),
                       Kr::titleRow("About"s),
                       Kr::rowContent(
                           Ui::button(
                               [](auto& n) {
                                   Ui::showDialog(n, Kr::aboutDialog("Ottercat"s));
                               },
                               "About Ottercat"
                           )
                       )
                   ) | Ui::vscroll() |
                       Ui::box({.backgroundFill = Ui::GRAY900}) | Ui::grow()
               ) | Ui::pinSize(280) |
                   Ui::slideIn(Ui::SlideFrom::END)
           ) |
           Ui::box({.backgroundFill = Gfx::BLACK.withOpacity(0.4)});
}

Ui::Child topBar() {
    return Ui::hflow(
               6,
               logo("Applications"),
               Ui::grow(NONE),
               statusWidget(Mdi::BROADCAST),
               statusWidget(Mdi::BATTERY_50, "50%"),
               statusWidget("11:29")
           ) |
           Ui::insets(12);
}

Ui::Child bottomBar(State const& s) {
    return Ui::hflow(
               6,
               buttonHint("  MENU  ", "SETTINGS", Ui::ACCENT500),
               Ui::grow(NONE),
               buttonHint("Y", "OPTIONS", Ui::GRAY50) | Ui::cond(not s.quickMenuVisible),
               buttonHint("A", "SELECT", Ui::GRAY50),
               buttonHint("B", "BACK", Ui::GRAY50)
           ) |
           Ui::insets(8);
}

Ui::Child appMenu() {
    return Kr::contextMenuContent({
        Kr::dialogHeader({
            Ui::hflow(
                12,
                Ui::image("bundle://hideo-handheld/tiles/celeste.qoi"_url, 6) |
                    Ui::box({
                        .borderRadii = 6,
                        .borderWidth = 1,
                        .borderFill = Ui::GRAY50.withOpacity(0.4),
                    }) |
                    Ui::pinSize(64) | Ui::vcenter(),
                Ui::vflow(
                    Kr::dialogTitle("Celeste"s),
                    Kr::dialogDescription("Version: v0.1"s),
                    Kr::dialogDescription("Size: 10.5MiB"s)
                )
            ),
        }),
        Kr::separator(),
        Kr::contextMenuItem(Ui::closeDialog, Mdi::PLAY, "Open"s),
        Kr::contextMenuItem(Ui::closeDialog, Mdi::PLAYLIST_REMOVE, "Remove from recents"s),
        Kr::contextMenuItem(Ui::closeDialog, Mdi::DELETE_FOREVER, "Uninstall"s),
        Kr::separator(),
        Kr::contextMenuItem(Ui::closeDialog, Mdi::CANCEL, "Cancel"s),
    });
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](State const& s) {
        return Ui::stack(
                   Ui::image("bundle://hideo-handheld/covers/celeste.qoi"_url) | Ui::foregroundFilter(Gfx::OverlayFilter{Ui::GRAY950.withOpacity(0.6)}) | Ui::cover(),
                   Ui::vflow(
                       topBar() |
                           Ui::box({
                               .backgroundFill = Ui::GRAY950.withOpacity(0.7),
                           }) |
                           Ui::backgroundFilter(Gfx::BlurFilter{8}),
                       Kr::separator(),
                       Ui::stack(appList(), s.quickMenuVisible ? quickSettings() : Ui::empty()) | Ui::grow(),
                       Kr::separator(),
                       bottomBar(s) |
                           Ui::box({
                               .backgroundFill = Ui::GRAY950.withOpacity(0.7),
                           }) |
                           Ui::backgroundFilter(Gfx::BlurFilter{8})
                   )
               ) |
               Ui::keyboardShortcut(App::Key::M, Model::bind<State::QuickMenuToggle>()) |
               Ui::keyboardShortcut(App::Key::ESC, Model::bind<State::QuickMenuToggle>()) |
               Ui::keyboardShortcut(App::Key::Y, [&](auto& n) {
                   if (not s.quickMenuVisible)
                       Ui::showDialog(n, appMenu() | Ui::center());
               }) |
               Ui::dialogLayer();
    });
}

} // namespace Hideo::Handheld

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    App::formFactor = App::FormFactor::MOBILE;

    co_return co_await Ui::runAsync(
        ctx,
        Hideo::Handheld::app() | Ui::pinSize({640, 480})
    );
}
