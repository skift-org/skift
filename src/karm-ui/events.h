#pragma once

#include <karm-meta/utils.h>

#include "keys.h"

namespace Karm::Ui {

struct Event {
    uint64_t _id{};
    bool _accepted{};

    Event(uint64_t id) : _id{id} {}

    template <typename T>
    Event &handle(auto callback) {
        if (_id == T::ID) {
            _accepted = callback(static_cast<T const &>(*this));
        }
        return *this;
    }
};

template <typename _Derived, uint64_t _ID>
struct _Event : public Event {
    using Derived = _Derived;
    static constexpr uint64_t ID = _ID;

    _Event() : Event{ID} {}
};

struct MouseEvent : public _Event<MouseEvent, 0x5db47c5474147944> {
    enum struct Type : uint8_t {
        PRESS,
        RELEASE,
        MOVE,
        SCOLL,

        ENTER,
        LEAVE
    };

    enum struct Button : uint8_t {
        LEFT,
        MIDDLE,
        RIGHT,
        X1,
        X2,

        _COUNT
    };

    enum struct Modifier : uint8_t {
        NONE,
        SHIFT,
        CTRL,
        ALT,
        SUPER,

        _COUNT
    };

    struct Axis {
        int x{};
        int y{};

        int vscroll{};
        int hscroll{};

        Axis() = default;
    };

    using enum Type;

    Type type;
    Axis screen;
    Axis local;
    Axis delta;
    bool buttons[static_cast<size_t>(Button::_COUNT)]{};
    bool modifiers[static_cast<size_t>(Modifier::_COUNT)]{};
    Button button{};

    bool hasModifiers(auto... modifier) const {
        return (modifiers[static_cast<size_t>(modifier)] || ...);
    }
};

struct KeyboardEvent : public _Event<KeyboardEvent, 0x1eb75d94f347352> {
    enum struct Type {
        PRESS,
        RELEASE,
    };

    using enum Type;

    Type type;
    Key key;
    Rune rune;
};

} // namespace Karm::Ui
