#pragma once

#include <karm-math/rect.h>
#include <karm-math/vec.h>
#include <karm-meta/utils.h>

#include "keys.h"

namespace Karm::Events {

struct Event {
    uint64_t id{};
    bool accepted{};

    Event(uint64_t id) : id{id} {}

    template <typename T>
    Event &handle(auto callback) {
        if (id == T::ID) {
            accepted = callback(static_cast<T const &>(*this));
        }
        return *this;
    }

    template <typename T>
    bool is() const {
        return id == T::ID;
    }

    template <typename T>
    T &unwrap() {
        return static_cast<T &>(*this);
    }

    template <typename T>
    T const &unwrap() const {
        return static_cast<T const &>(*this);
    }

    void accept() {
        accepted = true;
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
    Math::Vec2i scrollLines;
    Math::Vec2f scrollPrecise;
    Math::Vec2i delta;

    Button buttons{};
    Mod mods{};
    Button button{};
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

struct PaintEvent : public _Event<PaintEvent, 0xe024fcf3253d5696> {
    Math::Recti bound;
};

struct LayoutEvent : public _Event<LayoutEvent, 0x9fb51942f16848b6> {
};

struct AnimateEvent : public _Event<AnimateEvent, 0xf21fe2676e487fc> {
};

struct ExitEvent : public _Event<ExitEvent, 0x925703d3bb8540f6> {
    Error error;

    ExitEvent(Error error) : error{error} {}
};

} // namespace Karm::Events
