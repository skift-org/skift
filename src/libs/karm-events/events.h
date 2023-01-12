#pragma once

#include <karm-math/rect.h>
#include <karm-math/vec.h>

#include "keys.h"

namespace Karm::Events {

struct Event {
    Meta::Id id{};
    bool accepted{false};

    Event(Meta::Id id) : id{id} {}

    template <typename T>
    Event &handle(auto callback) {
        if (id == Meta::makeId<T>()) {
            accepted = accepted || callback(static_cast<T const &>(*this));
        }
        return *this;
    }

    template <typename T>
    bool is() const {
        return id == Meta::makeId<T>();
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

template <typename Crtp>
struct _Event : public Event {
    _Event() : Event{Meta::makeId<Crtp>()} {}
};

struct MouseEvent : public _Event<MouseEvent> {
    enum Type : uint8_t {
        PRESS,
        RELEASE,
        SCROLL,

        MOVE,
        ENTER,
        LEAVE
    };

    Type type;

    Math::Vec2i pos;
    Math::Vec2i scrollLines;
    Math::Vec2f scrollPrecise;
    Math::Vec2i delta;

    Button buttons{};
    Mod mods{};
    Button button{};
};

struct KeyboardEvent : public _Event<KeyboardEvent> {
    enum Type {
        PRESS,
        RELEASE,
    };

    Type type;
    Key key;
    Rune rune;
};

struct PaintEvent : public _Event<PaintEvent> {
    Math::Recti bound;

    PaintEvent(Math::Recti bound)
        : bound{bound} {}
};

struct LayoutEvent : public _Event<LayoutEvent> {};

struct BuildEvent : public _Event<BuildEvent> {};

struct AnimateEvent : public _Event<AnimateEvent> {};

struct ExitEvent : public _Event<ExitEvent> {
    Error error;

    ExitEvent(Error error)
        : error{error} {}
};

} // namespace Karm::Events
