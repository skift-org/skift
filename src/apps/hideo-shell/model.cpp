#include "app.h"

namespace Shell {

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](ToggleTablet) {
            state.isMobile = not state.isMobile;
            state.activePanel = Panel::NIL;
            state.isSysPanelColapsed = true;
            return state;
        },
        [&](Lock) {
            state.locked = true;
            state.activePanel = Panel::NIL;
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
        [&](ToggleSysPanel) {
            state.isSysPanelColapsed = not state.isSysPanelColapsed;
            return state;
        },
    });
}

} // namespace Shell
