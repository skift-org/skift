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
        [&](DimisNoti dismis) {
            s.noti.removeAt(dismis.index);
        },
        [&](StartApp start) {
            s.activePanel = Panel::NIL;
            s.surfaces.emplaceFront(
                0,
                s.entries[start.index],
                Gfx::randomColor());
        },
        [&](CloseApp close) {
            s.surfaces.removeAt(close.index);
        },
        [&](FocusApp focus) {
            auto surface = s.surfaces.removeAt(focus.index);
            s.surfaces.pushFront(surface);
            s.activePanel = Panel::NIL;
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
