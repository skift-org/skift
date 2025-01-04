#pragma once

#include <karm-app/form-factor.h>
#include <karm-gfx/icon.h>
#include <karm-image/picture.h>
#include <karm-ui/reducer.h>
#include <mdi/application.h>

namespace Hideo::Shell {

struct State;
struct Launcher;

// MARK: Notification ----------------------------------------------------------

struct Noti {
    usize id;
    String title;
    String body;
    Vec<String> actions{};
};

// MARK: Manifest & Instance ---------------------------------------------------

struct Launcher {
    Mdi::Icon icon = Mdi::APPLICATION;
    String name;
    Gfx::ColorRamp ramp;

    Launcher(Mdi::Icon icon, String name, Gfx::ColorRamp ramp)
        : icon(icon), name(name), ramp(ramp) {}

    virtual ~Launcher() = default;

    virtual void launch(State &) = 0;
};

struct Instance {
    usize id;
    Math::Recti bound = {100, 100, 600, 400};

    Instance() = default;

    virtual ~Instance() = default;

    virtual Ui::Child build() const = 0;
};

// MARK: Model -----------------------------------------------------------------

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
    Vec<Strong<Launcher>> launchers;
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

struct AddInstance {
    Strong<Instance> instance;
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
    AddInstance,
    MoveInstance,
    CloseInstance,
    FocusInstance,
    Activate,
    ToggleSysPanel,
    ToggleAppThumbnail>;

Ui::Task<Action> reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Shell
