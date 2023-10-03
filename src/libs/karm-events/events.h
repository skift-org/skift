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
    Math::Vec2i pos;
    Math::Vec2f scroll;
    Math::Vec2i delta;
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
    Key key;
};

struct TypedEvent {
    u32 codepoint;
};

struct ExitEvent {
    Res<> res = Ok();
};

struct RequestMinimizeEvent {
};

struct RequestMaximizeEvent {
};

} // namespace Karm::Events
