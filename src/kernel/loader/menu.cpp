#include <karm-fmt/case.h>
#include <karm-ui/app.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "loader.h"

namespace Loader {

/* --- Model ---------------------------------------------------------------- */

struct State {
    Configs configs;
    Opt<String> error = NONE;
    usize selected = 0;
};

struct MoveSelectionAction {
    isize delta;
};

struct SelectAction {};

using Actions = Var<MoveSelectionAction, SelectAction>;

State reduce(State state, Actions a) {
    return a.visit(Visitor{
        [&](MoveSelectionAction a) {
            if (state.selected == 0 && a.delta < 0)
                state.selected = state.configs.entries.len() - 1;
            else if (state.selected == state.configs.entries.len() - 1 && a.delta > 0)
                state.selected = 0;
            else
                state.selected += a.delta;
            return state;
        },
        [&](SelectAction) {
            if (state.error) {
                state.error = NONE;
                return state;
            }

            auto res = Loader::loadEntry(state.configs.entries[state.selected]);
            if (not res) {
                state.error = String{res.none().msg()};
            }
            return state;
        },
    });
}

void intent(Ui::Node &n, Events::Event &e) {
    e.handle<Events::KeyboardEvent>([&](auto &k) {
        if (Op::eq(k.key, Events::Key::LEFT)) {
            Ui::dispatchAction<Actions>(n, MoveSelectionAction{-1});
            return true;
        } else if (Op::eq(k.key, Events::Key::RIGHT)) {
            Ui::dispatchAction<Actions>(n, MoveSelectionAction{1});
            return true;
        } else if (Op::eq(k.key, Events::Key::ENTER)) {
            Ui::dispatchAction<Actions>(n, SelectAction{});
            return true;
        }

        return false;
    });
}

using Model = Ui::Model<State, Actions, reduce, intent>;

/* --- Views ---------------------------------------------------------------- */

Ui::Child icon(Entry const &e) {
    return e.icon.visit(Visitor{
        [&](Mdi::Icon i) {
            return Ui::icon(i, 64);
        },
        [&](Media::Image i) {
            return Ui::pinSize(64, Ui::image(i));
        },
        [&](None) {
            return Ui::empty();
        }});
}

Ui::Child entry(State const &s, Entry const &e, usize i) {
    auto style =
        s.selected == i
            ? Ui::ButtonStyle::regular()
            : Ui::ButtonStyle::secondary();

    auto tile = Ui::vflow(
        12,
        Layout::Align::CENTER,
        icon(e),
        Ui::titleMedium(e.name));

    return tile |
           Ui::center() |
           Ui::pinSize({192, 128}) |
           Ui::spacing(12) |
           Ui::button(Ui::NOP, style);
}

Ui::Child list(State const &s) {

    auto items =
        iter(s.configs.entries)
            .mapi([&](Entry const &e, usize i) {
                return entry(s, e, i);
            })
            .collect<Vec<Ui::Child>>();

    return Ui::hflow(16, items);
}

Ui::Child alert(String title, String subtitle) {
    auto dialog = Ui::vflow(
                      16,
                      Layout::Align::CENTER,
                      Ui::icon(Mdi::ALERT_DECAGRAM, 48),
                      Ui::titleLarge(title),
                      Ui::bodyMedium(Fmt::toSentenceCase(subtitle).unwrap())) |
                  Ui::box(Ui::BoxStyle{
                      .foregroundPaint = Ui::GRAY500,
                  }) |
                  Ui::center();

    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               dialog | Ui::grow(),
               Ui::bodyMedium("Press [ENTER] to continue.")) |
           Ui::spacing(64);
}

Ui::Child menu(Configs const &c) {
    if (c.entries.len() == 0)
        return alert("No entries found.", "Please add an entry to the configuration file.");

    return Ui::reducer<Model>(
        {c},
        [](State const &s) {
            if (s.error)
                return alert("System startup failed.", s.error.unwrap());

            return Ui::vflow(
                       16,
                       Layout::Align::CENTER,
                       Ui::headlineLarge(s.configs.title ? *s.configs.title : "Welcome!"),
                       Ui::titleLarge(Ui::GRAY400, s.configs.subtitle ? *s.configs.subtitle : "Select an operating system"),
                       list(s) | Ui::grow(4),
                       Ui::labelMedium("Use the [ARROW KEYS] to navigate, and press [ENTER] to select an entry."),
                       Ui::labelLarge(Ui::GRAY500, "Powered by opstart")) |
                   Ui::spacing(64);
        });
}

Res<> showMenu(Ctx &ctx, Configs const &c) {
    return Ui::runApp(ctx, Loader::menu(c));
}

} // namespace Loader
