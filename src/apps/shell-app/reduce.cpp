#include "app.h"

namespace Shell {

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](ToggleTablet) {
            state.isTablet = not state.isTablet;
            return state;
        },
        [&](Unlock) {
            state.locked = false;
            return state;
        },
        [&](Activate panel) {
            if (state.activePanel != panel.panel) {
                state.activePanel = panel.panel;
            } else {
                state.activePanel = Panel::NIL;
            }
            return state;
        },
    });
}

} // namespace Shell
