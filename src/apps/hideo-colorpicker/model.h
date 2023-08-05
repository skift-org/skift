#pragma once

#include <karm-ui/reducer.h>

namespace ColorPicker {

enum struct Page {
    HSV,
    PALLETE,
};

struct State {
    Page page = Page::HSV;
    Gfx::Hsv hsv;
};

struct UpdatePage {
    Page page;
};

struct UpdateHsv {
    Gfx::Hsv hsv;
};

using Action = Var<UpdatePage, UpdateHsv>;

inline State reduce(State state, Action action) {
    return action.visit(Visitor{
        [&](UpdatePage update) {
            state.page = update.page;
            return state;
        },
        [&](UpdateHsv update) {
            state.hsv = update.hsv;
            return state;
        },
    });
}

using Model = Karm::Ui::Model<State, Action, reduce>;

} // namespace ColorPicker
