#include <karm-kira/error-page.h>
#include <karm-ui/app.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/view.h>
#include <mdi/alert-decagram.h>

#include "loader.h"

namespace Opstart {

// MARK: Model -----------------------------------------------------------------

struct State {
    Configs configs;
    Opt<String> error = NONE;
    usize selected = 0;
};

struct MoveSelectionAction {
    isize delta;
};

struct SelectAction {};

using Action = Union<MoveSelectionAction, SelectAction>;

static Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](MoveSelectionAction a) {
            if (s.selected == 0 and a.delta < 0)
                s.selected = s.configs.entries.len() - 1;
            else if (s.selected == s.configs.entries.len() - 1 and a.delta > 0)
                s.selected = 0;
            else
                s.selected += a.delta;
        },
        [&](SelectAction) {
            if (s.error)
                s.error = NONE;
            auto res = Opstart::loadEntry(s.configs.entries[s.selected]);
            if (not res)
                s.error = String{res.none().msg()};
        },
    });

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

// MARK: Views -----------------------------------------------------------------

Ui::Child icon(Entry const& e) {
    return e.icon.visit(Visitor{
        [&](Mdi::Icon i) {
            return Ui::icon(i, 64);
        },
        [&](Image::Picture i) {
            return Ui::pinSize(64, Ui::image(i));
        },
        [&](None) {
            return Ui::empty();
        }
    });
}

Ui::Child entry(State const& s, Entry const& e, usize i) {
    auto style =
        s.selected == i
            ? Ui::ButtonStyle::regular()
            : Ui::ButtonStyle::secondary();

    auto tile = Ui::vflow(
        12,
        Math::Align::CENTER,
        icon(e),
        Ui::titleMedium(e.name)
    );

    return tile |
           Ui::center() |
           Ui::pinSize({192, 128}) |
           Ui::insets(12) |
           Ui::button(Ui::NOP, style);
}

Ui::Child list(State const& s) {

    auto items =
        iter(s.configs.entries)
            .mapi([&](Entry const& e, usize i) {
                return entry(s, e, i);
            })
            .collect<Vec<Ui::Child>>();

    return Ui::hflow(16, items);
}

Ui::Child alert(String title, String subtitle) {
    return Ui::vflow(
               16,
               Math::Align::CENTER,
               Kr::errorPage(Mdi::ALERT_DECAGRAM, title, subtitle) | Ui::grow(),
               Ui::bodyMedium("Press [ENTER] to continue.")
           ) |
           Ui::insets(64);
}

void intent(Ui::Node& n, App::Event& e) {
    if (auto k = e.is<App::KeyboardEvent>()) {
        if (k->key == App::Key::LEFT) {
            Ui::bubble<Action>(n, MoveSelectionAction{-1});
            e.accept();
        } else if (k->key == App::Key::RIGHT) {
            Ui::bubble<Action>(n, MoveSelectionAction{1});
            e.accept();
        } else if (k->key == App::Key::ENTER) {
            Ui::bubble<Action>(n, SelectAction{});
            e.accept();
        }
    }
}

Ui::Child menu(Configs const& c) {
    if (c.entries.len() == 0)
        return alert(
            "No entries found."s,
            "Please add an entry to the configuration file."s
        );

    return Ui::reducer<Model>(
        {c},
        [](State const& s) {
            if (s.error)
                return alert(
                    "System startup failed."s,
                    s.error.unwrap()
                );

            Ui::Children children;

            if (s.configs.title)
                children.pushBack(Ui::headlineLarge(*s.configs.title));

            if (s.configs.subtitle)
                children.pushBack(Ui::titleLarge(Ui::GRAY400, *s.configs.subtitle));

            children.pushBack(list(s) | Ui::grow());

            children.pushBack(Ui::labelMedium("Use ← and → to navigate, and press ENTER to select an entry."));

            children.pushBack(Ui::labelSmall(Ui::GRAY500, "Powered by Opstart ►"));

            return Ui::vflow(
                       16,
                       Math::Align::CENTER,
                       children
                   ) |
                   Ui::insets(64) |
                   Ui::intent(intent);
        }
    );
}

Res<> showMenu(Sys::Context& ctx, Configs const& c) {
    return Ui::runApp(ctx, Opstart::menu(c));
}

} // namespace Opstart
