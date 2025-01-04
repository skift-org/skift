#include "app.h"

namespace Hideo::Shell {

Ui::Task<Action> reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](ToggleTablet) {
            s.isMobile = not s.isMobile;
            s.activePanel = Panel::NIL;
            s.isSysPanelColapsed = true;
        },
        [&](ToggleNightLight) {
            s.nightLight = not s.nightLight;
        },
        [&](ChangeBrightness m) {
            s.brightness = m.value;
        },
        [&](ChangeVolume m) {
            s.volume = m.value;
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
        [&](StartInstance start) {
            s.launchers[start.index]->launch(s);
            s.activePanel = Panel::NIL;
        },
        [&](AddInstance add) {
            s.instances.pushBack(add.instance);
        },
        [&](MoveInstance move) {
            s.activePanel = Panel::NIL;
            auto bound = s.instances[move.index]->bound;
            bound.xy = bound.xy + move.off;
            s.instances[move.index]->bound = bound;
        },
        [&](CloseInstance close) {
            s.instances.removeAt(close.index);
        },
        [&](FocusInstance focus) {
            auto surface = s.instances.removeAt(focus.index);
            s.instances.pushFront(surface);
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

    return NONE;
}

} // namespace Hideo::Shell
