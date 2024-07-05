#pragma once

#include <karm-image/picture.h>
#include <karm-ui/reducer.h>

#include "instance.h"

namespace Hideo::Shell {

struct Noti {
    usize id;
    String title;
    String body;
    Vec<String> actions{};
    Strong<Manifest> manifest;
};

enum struct Panel {
    NIL,
    APPS,
    NOTIS,
    SYS,
};

struct State : Meta::NoCopy {
    bool locked = true;
    bool isMobile = true;
    bool nightLight = false;
    f64 brightness = 0.75;
    f64 volume = 0.5;
    Panel activePanel = Panel::NIL;
    bool isSysPanelColapsed = true;
    bool isAppPanelThumbnails = false;

    DateTime dateTime;

    Image::Picture background;
    Vec<Noti> noti;
    Vec<Strong<Manifest>> manifests;
    Vec<Strong<Instance>> instances;
};

struct ToggleTablet {};

struct ToggleNightLight {};

struct ChangeBrightness {
    f64 value;
};

struct ChangeVolume {
    f64 value;
};

struct Lock {};

struct Unlock {};

struct DimisNoti {
    usize index;
};

struct StartInstance {
    usize index;
};

struct MoveInstance {
    usize index;
    Math::Vec2i off;
};

struct CloseInstance {
    usize index;
};

struct FocusInstance {
    usize index;
};

struct ToggleSysPanel {};

struct ToggleAppThumbnail {
    bool value;
};

struct Activate {
    Panel panel;
};

using Action = Union<
    ToggleTablet,
    ToggleNightLight,
    ChangeBrightness,
    ChangeVolume,
    Lock,
    Unlock,
    DimisNoti,
    StartInstance,
    MoveInstance,
    CloseInstance,
    FocusInstance,
    Activate,
    ToggleSysPanel,
    ToggleAppThumbnail>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Shell
