#pragma once

#include <karm-base/enum.h>
#include <karm-base/string.h>

namespace Karm::Events {

// MARK: Mouse ---------------------------------------------

enum struct Button : u8 {
    NONE = 0,

    LEFT = 1 << 0,
    MIDDLE = 1 << 1,
    RIGHT = 1 << 2,
    X1 = 1 << 3,
    X2 = 1 << 4,
};

FlagsEnum$(Button);

// MARK: Keyboard ------------------------------------------

enum struct Mod : u16 {
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

    SHIFT = LSHIFT | RSHIFT,
    CTRL = LCTRL | RCTRL,
    ALT = LALT | RALT,
    SUPER = LSUPER | RSUPER,
};

FlagsEnum$(Mod);

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

    auto operator<=>(Key const &other) const = default;

    auto operator<=>(Code const &other) const {
        return _code <=> other;
    }
};

} // namespace Karm::Events
