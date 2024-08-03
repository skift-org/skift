#pragma once

#include <karm-base/box.h>
#include <karm-meta/id.h>

#include "event.h"

namespace Karm::App {

// MARK: Event -----------------------------------------------------------------

struct Event {
    bool _accepted = false;

    virtual ~Event() = default;

    virtual Meta::Id id() const = 0;

    virtual void *_unwrap() = 0;
    virtual void const *_unwrap() const = 0;

    template <typename T>
    T *is() {
        return id() == Meta::idOf<T>() ? &unwrap<T>() : nullptr;
    }

    template <typename T>
    T const *is() const {
        return id() == Meta::idOf<T>() ? &unwrap<T>() : nullptr;
    }

    template <typename T>
    T &unwrap() {
        return *static_cast<T *>(_unwrap());
    }

    template <typename T>
    T const &unwrap() const {
        return *static_cast<T const *>(_unwrap());
    }

    void accept() {
        _accepted = true;
    }

    bool accepted() {
        return _accepted;
    }
};

template <typename T>
struct _Event : public Event {
    T _buf;

    template <typename... Args>
    _Event(Args &&...args)
        : _buf{std::forward<Args>(args)...} {}

    Meta::Id id() const override {
        return Meta::idOf<T>();
    }

    void *_unwrap() override {
        return &_buf;
    }

    void const *_unwrap() const override {
        return &_buf;
    }
};

template <typename T, typename... Args>
Box<Event> makeEvent(Args &&...args) {
    return makeBox<_Event<T>>(std::forward<Args>(args)...);
}

// MARK: Dispatch --------------------------------------------------------------

struct Dispatch {
    virtual ~Dispatch() = default;

    virtual void event(App::Event &) = 0;

    virtual void bubble(App::Event &) = 0;
};

} // namespace Karm::App
