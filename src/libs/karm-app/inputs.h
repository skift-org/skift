#pragma once

#include <karm-base/enum.h>
#include <karm-math/vec.h>

#include "event.h"

namespace Karm::App {

// MARK: Keyboard --------------------------------------------------------------

enum struct KeyMod : u16 {
    NONE = 0,

    LSHIFT = 1 << 0,
    RSHIFT = 1 << 1,
    LCTRL = 1 << 2,
    RCTRL = 1 << 3,
    LALT = 1 << 4,
    RALT = 1 << 5,
    LSUPER = 1 << 6,
    RSUPER = 1 << 7,

    NUM = 1 << 8,
    CAPS = 1 << 9,
    MODE = 1 << 10,
    SCROLL = 1 << 11,

    // Either left or right modifier
    SHIFT = 1 << 12,
    CTRL = 1 << 13,
    ALT = 1 << 14,
    SUPER = 1 << 15,
};

FlagsEnum$(KeyMod);

static inline bool match(KeyMod in, KeyMod mods) {
    KeyMod either = KeyMod::NONE;
    KeyMod mask = KeyMod::NONE;

    // Removing the non-combinable modifiers from the matching
    // Because eg: NumLock + A should be considered exactly like A
    if (static_cast<bool>(in & KeyMod::NUM)) {
        mask |= KeyMod::NUM;
    }
    if (static_cast<bool>(in & KeyMod::CAPS)) {
        mask |= KeyMod::CAPS;
    }
    if (static_cast<bool>(in & KeyMod::MODE)) {
        mask |= KeyMod::MODE;
    }
    if (static_cast<bool>(in & KeyMod::SCROLL)) {
        mask |= KeyMod::SCROLL;
    }

    if (static_cast<bool>(in & KeyMod::LSHIFT) or
        static_cast<bool>(in & KeyMod::RSHIFT)) {
        either |= KeyMod::SHIFT;
        mask |= KeyMod::LSHIFT | KeyMod::RSHIFT;
    }

    if (static_cast<bool>(in & KeyMod::LCTRL) or
        static_cast<bool>(in & KeyMod::RCTRL)) {
        either |= KeyMod::CTRL;
        mask |= KeyMod::LCTRL | KeyMod::RCTRL;
    }

    if (static_cast<bool>(in & KeyMod::LALT) or
        static_cast<bool>(in & KeyMod::RALT)) {
        either |= KeyMod::ALT;
        mask |= KeyMod::LALT | KeyMod::RALT;
    }

    if (static_cast<bool>(in & KeyMod::LSUPER) or
        static_cast<bool>(in & KeyMod::RSUPER)) {
        either |= KeyMod::SUPER;
        mask |= KeyMod::LSUPER | KeyMod::RSUPER;
    }

    return (((in | either) & mods) == mods) and
           ((in & (mods | mask)) == in);
}

enum struct KeyMotion {
    RELEASED,
    PRESSED,
};

struct Key {
    enum struct Code {
#define KEY(name, code) name = code,
#include "defs/keys.inc"

#undef KEY
    };

    using enum Code;

    Code _code;

    Key(Code code = Code::INVALID)
        : _code(code) {}

    Str name() const {
        switch (_code) {
#define KEY(name, code) \
    case Code::name:    \
        return #name;
#include "defs/keys.inc"

#undef KEY
        }
        return "INVALID";
    }

    Code code() {
        return _code;
    }

    auto operator<=>(Key const& other) const = default;

    auto operator<=>(Code const& other) const {
        return _code <=> other;
    }
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

    KeyMod mods = KeyMod::NONE;
};

struct TypeEvent {
    Rune rune;
    KeyMod mods = KeyMod::NONE;
};

// MARK: Mouse -----------------------------------------------------------------

enum struct MouseButton : u8 {
    NONE = 0,

    LEFT = 1 << 0,
    MIDDLE = 1 << 1,
    RIGHT = 1 << 2,
    X1 = 1 << 3,
    X2 = 1 << 4,
};

FlagsEnum$(MouseButton);

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
    MouseButton buttons{};
    KeyMod mods{};
    MouseButton button{};

    bool pressed(MouseButton button) const {
        return (bool)(buttons & button);
    }

    bool released(MouseButton button) const {
        return not pressed(button);
    }
};

struct MouseLeaveEvent {
};

struct MouseEnterEvent {
};

} // namespace Karm::App
