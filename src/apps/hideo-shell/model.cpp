module;

#include <karm-gfx/icon.h>

export module Hideo.Shell:model;

import Mdi;
import Karm.Ui;
import Karm.App;
import Karm.Image;

using namespace Karm;

namespace Hideo::Shell {

export struct State;

// MARK: Notification ----------------------------------------------------------

export struct Noti {
    usize id;
    String title;
    String body;
    Vec<String> actions{};
};

// MARK: Manifest & Instance ---------------------------------------------------

export struct Launcher {
    Gfx::Icon icon = Mdi::APPLICATION;
    String name;
    Gfx::ColorRamp ramp;

    Launcher(Gfx::Icon icon, String name, Gfx::ColorRamp ramp)
        : icon(icon), name(name), ramp(ramp) {}

    virtual ~Launcher() = default;

    virtual void launch(State&) = 0;
};

export struct Instance {
    usize id;
    Math::Recti bound = {100, 100, 600, 400};

    Instance() = default;

    virtual ~Instance() = default;

    virtual Ui::Child build() const = 0;
};

// MARK: Model -----------------------------------------------------------------

export enum struct Panel {
    NIL,
    APPS,
    NOTIS,
    SYS,
};

export struct State : Meta::NoCopy {
    bool locked = true;
    bool isMobile = true;
    bool nightLight = false;
    f64 brightness = 1;
    f64 volume = 0.5;
    Panel activePanel = Panel::NIL;
    bool isSysPanelColapsed = true;
    bool isAppPanelThumbnails = false;

    DateTime dateTime;

    Image::Picture background;
    Vec<Noti> noti;
    Vec<Rc<Launcher>> launchers;
    Vec<Rc<Instance>> instances;
};

export struct ToggleTablet {};

export struct ToggleNightLight {};

export struct ChangeBrightness {
    f64 value;
};

export struct ChangeVolume {
    f64 value;
};

export struct Lock {};

export struct Unlock {};

export struct DimisNoti {
    usize index;
};

export struct StartInstance {
    usize index;
};

export struct AddInstance {
    Rc<Instance> instance;
};

export struct MoveInstance {
    usize index;
    Math::Vec2i off;
};

export struct CloseInstance {
    usize index;
};

export struct FocusInstance {
    usize index;
};

export struct ToggleSysPanel {};

export struct ToggleAppThumbnail {
    bool value;
};

export struct Activate {
    Panel panel;
};

export using Action = Union<
    ToggleTablet,
    ToggleNightLight,
    ChangeBrightness,
    ChangeVolume,
    Lock,
    Unlock,
    DimisNoti,
    StartInstance,
    AddInstance,
    MoveInstance,
    CloseInstance,
    FocusInstance,
    Activate,
    ToggleSysPanel,
    ToggleAppThumbnail>;

Ui::Task<Action> reduce(State& s, Action a) {
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

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Shell
