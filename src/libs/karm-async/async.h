#pragma once

#include <karm-base/box.h>
#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-meta/nocopy.h>

#include "_embed.h"

namespace Karm::Async {

/* --- Event ---------------------------------------------------------------- */

enum struct Propagation {
    NONE, //< Don't propagate futher than the target sink
    UP,   //< Move down the children of the target sink
    DOWN, //< Move up the parent of the target sink
    STOP, //< Stop the propagation
};

struct Event {
    Propagation _propagation;

    Event(Propagation propagation)
        : _propagation(propagation) {}

    virtual ~Event() = default;

    virtual Meta::Id id() const = 0;
    virtual Opt<Bytes> bytes() const { return NONE; }

    virtual void *_unwrap() = 0;
    virtual void const *_unwrap() const = 0;

    template <typename T>
    Event &handle(auto callback) {
        if (auto *p = is<T>())
            if (callback(*p)) {
                accept();
            }
        return *this;
    }

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
        _propagation = Propagation::STOP;
    }

    bool accepted() {
        return _propagation == Propagation::STOP;
    }
};

template <typename T>
struct _Event : public Event {
    T _buf;

    template <typename... Args>
    _Event(Propagation p, Args &&...args)
        : Event(p), _buf{std::forward<Args>(args)...} {}

    Meta::Id id() const override {
        return Meta::idOf<T>();
    }

    void *_unwrap() override {
        return &_buf;
    }

    void const *_unwrap() const override {
        return &_buf;
    }

    Opt<Bytes> bytes() const override {
        if constexpr (Meta::TrivialyCopyable<T>) {
            return Bytes{reinterpret_cast<Byte const *>(&_buf), sizeof(T)};
        }
        return NONE;
    }
};

template <typename T, typename... Args>
Box<Event> makeEvent(Propagation propagation, Args &&...args) {
    return makeBox<_Event<T>>(propagation, std::forward<Args>(args)...);
}

/* --- Loop ----------------------------------------------------------------- */

struct Sink;
struct Source;

struct Loop : public Meta::Static {
    struct Queued {
        Sink *sink;
        Box<Event> event;
    };

    struct Binding {
        Source *source;
        Sink *sink;
    };

    virtual ~Loop() = default;

    Opt<Res<>> _ret;
    Vec<Queued> _queued;
    Vec<Binding> _sources;

    /* --- Sink --- */

    void _post(Sink &sink, Box<Event> event);

    void _move(Sink &from, Sink *to);

    /* --- Source --- */

    void _bind(Source &source, Sink &sink);

    void _move(Source &from, Source *to);

    /* --- Public --- */

    void _collect();

    Res<TimeStamp> poll();

    Res<usize> dispatch();

    virtual TimeStamp now() = 0;

    virtual Res<> wait(TimeStamp until) = 0;

    Res<> run();

    void quit(Res<> ret) { _ret = ret; }

    template <typename E, typename... Args>
    void post(Sink &sink, Args &&...args) {
        _post(sink, makeEvent<E>(Propagation::NONE, std::forward<Args>(args)...));
    }
};

static Loop &loop() { return _Embed::loop(); }

/* --- Sink ----------------------------------------------------------------- */

struct Sink : public Meta::NoCopy {

    Sink() = default;

    Sink(Sink &&other) {
        loop()._move(other, this);
    }

    Sink &operator=(Sink &&other) {
        loop()._move(other, this);
        return *this;
    }

    virtual ~Sink() {
        loop()._move(*this, nullptr);
    }

    // This needs to be protected to avoid Source from posting Event to a Sink directly.
protected:
    virtual Res<> post(Event &) = 0;
    friend struct Loop;
};

/* --- Source --------------------------------------------------------------- */

struct Source : public Meta::NoCopy {
    Source(Sink &sink) {
        loop()._bind(*this, sink);
    }

    Source(Source &&other) {
        loop()._move(other, this);
    }

    Source &operator=(Source &&other) {
        loop()._move(other, this);
        return *this;
    }

    virtual ~Source() {
        loop()._move(*this, nullptr);
    }

    virtual Res<TimeStamp> poll(Sink &) = 0;
};

/* --- Task ----------------------------------------------------------------- */

template <typename T = void>
using Coro = std::coroutine_handle<T>;

template <typename T = Ok<>>
struct Task : public Sink {
    using Inner = T;

    struct RetEvent {
        T res;
    };

    struct Promise {
        Sink *_sink;
        Task get_return_object() { return {Coro<Promise>::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_value(T res) {
            if (_sink)
                loop().post<RetEvent>(*_sink, std::move(res));
        }
        void bind(Sink &sink) { _sink = &sink; }
    };

    Coro<Promise> _coro;
    Coro<> _wait;
    Opt<T> _ret;

    Task(Coro<Promise> coro)
        : _coro(coro) {}

    Task(Task &&other)
        : _coro(std::exchange(other._coro, {})) {}

    ~Task() {
        if (_coro)
            _coro.destroy();
    }

    Promise &promise() {
        return _coro.promise();
    }

    void bind(Sink &sink) {
        promise().bind(sink);
    }

    Res<> post(Event &e) override {
        _ret = std::move(e.unwrap<RetEvent>().res);
        _wait.resume();
        return Ok();
    }

    // Theses methods and types are required by the coroutine machinery

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(Coro<> coro) noexcept {
        _wait = coro;
        bind(*this);
    }

    T await_resume() noexcept {
        return _ret.take();
    }

    using promise_type = Promise;
};

template <typename... Args>
inline Task<Tuple<typename Args::Inner...>> all(Args &...tasks) {
    co_return {co_await tasks...};
}

/* --- Defer ---------------------------------------------------------------- */

template <typename Source>
struct Awaitable : public Sink {
    Source _source;
    Coro<> _coro = nullptr;

    template <typename... Args>
    Awaitable(Args... args)
        : Sink{}, _source{*this, std::forward<Args>(args)...} {
    }

    constexpr bool await_ready() const {
        return false;
    }

    void await_suspend(Coro<> coro) {
        _coro = coro;
    }

    void await_resume() const {}

    Res<> post(Async::Event &) override {
        if (not _coro)
            panic("no coro");

        auto coro = std::exchange(_coro, nullptr);
        coro.resume();
        return Ok();
    }
};

struct Defer : public Source {
    using Source::Source;

    struct Event {
    };

    bool _once = false;

    void defer() {
        _once = false;
    }

    Res<TimeStamp> poll(Sink &sink) override {
        if (not _once) {
            loop().post<Event>(sink);
            _once = true;
        }
        return Ok(TimeStamp::endOfTime());
    };
};

static inline void defer(Sink &sink) {
    loop().post<Defer::Event>(sink);
}

static inline Awaitable<Defer> defer() {
    return {};
}

/* --- Timer ---------------------------------------------------------------- */

struct Timer : public Source {
    struct TickEvent {
    };

    TimeStamp _next;
    Opt<TimeSpan> _repeat;
    bool _active = true;

    Timer(Sink &sink, TimeStamp next, Opt<TimeSpan> repeat)
        : Source(sink),
          _next{next},
          _repeat{repeat} {}

    Res<TimeStamp> poll(Sink &sink) override {
        if (not _active)
            return Ok(TimeStamp::endOfTime());

        if (_next < loop().now()) {
            loop().post<TickEvent>(sink);

            if (not _repeat) {
                _active = false;
                return Ok(TimeStamp::endOfTime());
            }

            _next += *_repeat;
        }

        return Ok(_next);
    };
};

static inline Timer every(Sink &sink, TimeSpan span) {
    return {sink, loop().now() + span, span};
}

static inline Timer after(Sink &sink, TimeSpan span) {
    return {sink, loop().now() + span, NONE};
}

static inline Timer after(Sink &sink, TimeStamp stamp) {
    return {sink, stamp, NONE};
}

static inline Awaitable<Timer> after(TimeStamp stamp) {
    return {stamp, NONE};
}

static inline Awaitable<Timer> after(TimeSpan span) {
    return {loop().now() + span, NONE};
}

} // namespace Karm::Async
