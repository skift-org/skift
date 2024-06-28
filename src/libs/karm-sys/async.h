#pragma once

#include <karm-base/async.h>
#include <karm-base/box.h>
#include <karm-base/map.h>

#include "fd.h"

namespace Karm::Sys {

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

// MARK: Scheduler -------------------------------------------------------------

struct Sched :
    Meta::Static {

    Opt<Res<>> _ret;

    virtual ~Sched() = default;

    bool exited() const { return _ret.has(); }

    void quit(Res<> ret) { _ret = ret; }

    virtual Res<> wait(TimeStamp until) = 0;

    virtual Async::Task<usize> readAsync(Strong<Fd>, MutBytes) = 0;

    virtual Async::Task<usize> writeAsync(Strong<Fd>, Bytes) = 0;

    virtual Async::Task<usize> flushAsync(Strong<Fd>) = 0;

    virtual Async::Task<_Accepted> acceptAsync(Strong<Fd>) = 0;

    virtual Async::Task<_Sent> sendAsync(Strong<Fd>, Bytes, Slice<Handle>, SocketAddr) = 0;

    virtual Async::Task<_Received> recvAsync(Strong<Fd>, MutBytes, MutSlice<Handle>) = 0;

    virtual Async::Task<> sleepAsync(TimeStamp until) = 0;
};

Sched &globalSched();

template <Async::Sender S>
auto run(S s, Sched &sched = globalSched()) {
    return Async::run(std::move(s), [&] {
        (void)sched.wait(TimeStamp::endOfTime());
    });
}

} // namespace Karm::Sys
