#pragma once

#include <karm-base/box.h>
#include <karm-math/rect.h>
#include <karm-math/vec.h>
#include <karm-meta/traits.h>

#include "keys.h"

namespace Karm::Events {

struct MouseEvent {
    enum {
        PRESS,
        RELEASE,
        SCROLL,
        MOVE,
    } type;

    Math::Vec2i pos{};
    Math::Vec2f scroll{};
    Math::Vec2i delta{};
    Button buttons{};
    Mod mods{};
    Button button{};
};

struct MouseLeaveEvent {
};

struct MouseEnterEvent {
};

struct KeyboardEvent {
    enum {
        PRESS,
        RELEASE,
    } type;

    /// The code of the key that was pressed or released
    /// This value takes the current keyboard layout into account
    Key key;

    /// The scancode of the key that was pressed or released
    /// This value is independent of the current keyboard layout
    Key code;

    Mod mods = Mod::NONE;
};

struct TypedEvent {
    u32 codepoint;
    Mod mods = Mod::NONE;
};

struct RequestExitEvent {
    Res<> res = Ok();
};

struct RequestMinimizeEvent {
};

struct RequestMaximizeEvent {
};

} // namespace Karm::Events
