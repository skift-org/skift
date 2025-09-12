export module Hideo.Keyboard:model;

import Karm.Core;
import Karm.Ui;

using namespace Karm;

namespace Hideo::Keyboard {

export struct State {
    bool shift = true;
};

export struct ToggleShift {
};

export using Action = Union<ToggleShift>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(
        Visitor{
            [&](ToggleShift) {
                s.shift = not s.shift;
            },
        }
    );

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Keyboard
