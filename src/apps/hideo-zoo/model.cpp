module;

#include <karm-gfx/icon.h>

export module Hideo.Zoo:model;

import Karm.Ui;

using namespace Karm;

namespace Hideo::Zoo {

struct Page {
    Gfx::Icon icon;
    Str name;
    Str description;
    Ui::Slot build;
};

struct State {
    usize page;
};

struct Switch {
    usize page;
};

export using Action = Union<Switch>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(
        [&](Switch action) {
            s.page = action.page;
        }
    );

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Zoo
