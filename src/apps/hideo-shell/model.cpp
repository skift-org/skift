#include "app.h"

namespace Hideo::Shell {

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
                Math::Recti{0, 0, 400, 300},
                Gfx::randomColor());
        },
        [&](MoveApp move) {
            logInfo("{}", move.off);
            s.activePanel = Panel::NIL;
            auto bound = s.surfaces[move.index].bound;
            bound.xy = bound.xy + move.off;
            s.surfaces[move.index].bound = bound;
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
        [&](ToggleAppThumbnail a) {
            s.isAppPanelThumbnails = a.value;
        },
    });
}

} // namespace Hideo::Shell
