#include "app.h"

namespace Shell {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](ToggleTablet) {
            s.isMobile = not s.isMobile;
            s.activePanel = Panel::NIL;
            s.isSysPanelColapsed = true;
        },
        [&](Lock) {
            s.locked = true;
            s.activePanel = Panel::NIL;
        },
        [&](Unlock) {
            s.locked = false;
        },
        [&](Activate panel) {
            if (s.activePanel != panel.panel) {
                s.activePanel = panel.panel;
            } else {
                s.activePanel = Panel::NIL;
            }
        },
        [&](ToggleSysPanel) {
            s.isSysPanelColapsed = not s.isSysPanelColapsed;
        },
    });
}

} // namespace Shell
