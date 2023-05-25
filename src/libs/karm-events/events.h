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
        if (id == Meta::idOf<T>()) {
            accepted = accepted or callback(static_cast<T const &>(*this));
        }
        return *this;
    }

    template <typename T>
    bool is() const {
        return id == Meta::idOf<T>();
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
struct BaseEvent : public Event {
    BaseEvent() : Event{Meta::idOf<Crtp>()} {}
};

struct MouseEvent : public BaseEvent<MouseEvent> {
    enum Type : u8 {
        PRESS,
        RELEASE,
        SCROLL,
        MOVE,
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

struct MouseLeaveEvent : public BaseEvent<MouseLeaveEvent> {};

struct MouseEnterEvent : public BaseEvent<MouseLeaveEvent> {};

struct KeyboardEvent : public BaseEvent<KeyboardEvent> {
    enum Type {
        PRESS,
        RELEASE,
    };

    Type type;
    Key key;
    Rune rune;
};

struct PaintEvent : public BaseEvent<PaintEvent> {
    Math::Recti bound;

    PaintEvent(Math::Recti bound)
        : bound{bound} {}
};

struct LayoutEvent : public BaseEvent<LayoutEvent> {};

struct BuildEvent : public BaseEvent<BuildEvent> {};

struct AnimateEvent : public BaseEvent<AnimateEvent> {
    f64 dt;

    AnimateEvent(f64 dt)
        : dt{dt} {}
};

struct ExitEvent : public BaseEvent<ExitEvent> {
    Res<> res = Ok();

    ExitEvent()
        : res{Ok()} {}

    ExitEvent(Res<> res)
        : res{res} {}
};

struct RequestMinimizeEvent :
    public BaseEvent<RequestMinimizeEvent> {};

struct RequestMaximizeEvent :
    public BaseEvent<RequestMaximizeEvent> {};

} // namespace Karm::Events
