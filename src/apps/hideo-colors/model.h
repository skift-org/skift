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

inline void reduce(State &s, Action action) {
    action.visit(Visitor{
        [&](UpdatePage update) {
            s.page = update.page;
        },
        [&](UpdateHsv update) {
            s.hsv = update.hsv;
        },
    });
}

using Model = Ui::Model<State, Action, reduce>;

} // namespace ColorPicker
