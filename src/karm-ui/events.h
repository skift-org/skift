#pragma once

#include <karm-math/vec.h>
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
        SCROLL,

        MOVE,
        ENTER,
        LEAVE
    };

    using enum Type;

    Type type;

    Math::Vec2i pos;
    Math::Vec2i scroll;
    Math::Vec2i delta;

    Input::Button buttons{};
    Input::Mod mods{};
    Input::Button button{};
};

struct KeyboardEvent : public _Event<KeyboardEvent, 0x1eb75d94f347352> {
    enum struct Type {
        PRESS,
        RELEASE,
    };

    using enum Type;

    Type type;
    Input::Key key;
    Rune rune;
};

} // namespace Karm::Ui
