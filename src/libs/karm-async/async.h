#pragma once

#include <karm-base/box.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-meta/nocopy.h>

#include "_embed.h"

namespace Karm::Async {

/* --- Event ---------------------------------------------------------------- */

struct Event {
    bool _accepted{false};

    virtual ~Event() = default;

    virtual Meta::Type<> type() const = 0;

    void accept() {
        _accepted = true;
    }

    template <typename T>
    T &unwrap() {
        return static_cast<T &>(*this);
    }

    template <typename T>
    T const &unwrap() const {
        return static_cast<T const &>(*this);
    }

    template <typename T>
    T const *is() const {
        return type() == Meta::typeOf<T>() ? &unwrap<T>() : nullptr;
    }

    template <typename T>
    T *is() {
        return type() == Meta::typeOf<T>() ? &unwrap<T>() : nullptr;
    }

    template <typename T>
    Event &handle(auto callback) {
        if (auto *e = is<T>()) {
            _accepted = _accepted or callback(*e);
        }
        return *this;
    }
};

template <typename Crtp>
struct BaseEvent : public Event {
    Meta::Type<Crtp> _type = Meta::typeOf<Crtp>();

    Meta::Type<> type() const override {
        return _type;
    }
};

/* --- Loop ----------------------------------------------------------------- */

struct Sink;
struct Source;

struct Loop : public Meta::Static {
    struct Queued {
        Sink *sink;
        Box<Event> event;

        // FIXME: Remove this once clang 16 become generaly available
        //        This is needed because clang 15 don't support
        //        parenthesized initialization of aggregates
        Queued(Sink &s, Box<Event> e)
            : sink(&s), event(std::move(e)) {}
    };

    struct Binding {
        Source *source;
        Sink *sink;

        // FIXME: Remove this once clang 16 become generaly available
        //        This is needed because clang 15 don't support
        //        parenthesized initialization of aggregates
        Binding(Source &src, Sink &sk)
            : source(&src), sink(&sk) {}
    };

    virtual ~Loop() = default;

    Opt<Res<>> _ret;
    Vec<Queued> _queued;
    Vec<Binding> _sources;

    /* --- Sink --- */

    void _post(Sink &sink, Box<Event> event);

    void _move(Sink &from, Sink &to);

    void _dtor(Sink &sink);

    /* --- Source --- */

    void _bind(Source &source, Sink &sink);

    void _move(Source &from, Source &to);

    void _dtor(Source *source);

    /* --- Public --- */

    void _collect();

    Res<TimeStamp> poll();

    Res<usize> dispatch();

    virtual TimeStamp now() = 0;

    virtual Res<> wait(TimeStamp until) = 0;

    Res<> run();

    void quit(Res<> ret) {
        _ret = ret;
    }

    template <typename E, typename... Args>
    void post(Sink &sink, Args &&...args) {
        _post(sink, makeBox<E>(std::forward<Args>(args)...));
    }
};

static Loop &loop() { return _Embed::loop(); }

/* --- Sink ----------------------------------------------------------------- */

struct Sink : public Meta::NoCopy {
    Sink() = default;

    Sink(Sink &&other) {
        loop()._move(other, *this);
    }

    Sink &operator=(Sink &&other) {
        loop()._move(other, *this);
        return *this;
    }

    virtual ~Sink() {
        loop()._dtor(*this);
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
        loop()._move(other, *this);
    }

    Source &operator=(Source &&other) {
        loop()._move(other, *this);
        return *this;
    }

    virtual ~Source() {
        loop()._dtor(this);
    }

    virtual Res<TimeStamp> poll(Sink &) = 0;
};

/* --- Task ----------------------------------------------------------------- */

template <typename T = void>
using Coro = std::coroutine_handle<T>;

template <typename T = Ok<>>
struct Task : public Sink {
    using Inner = T;

    struct Ret : public BaseEvent<Ret> {
        T res;

        Ret(T r) : BaseEvent<Ret>(), res(r) {}
    };

    struct Promise {
        Sink *_sink;
        Task get_return_object() { return {Coro<Promise>::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_value(T res) {
            if (_sink)
                loop().post<Ret>(*_sink, std::move(res));
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
        _ret = std::move(e.unwrap<Ret>().res);
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

struct Defer : public BaseEvent<Event> {
};

static inline void defer(Sink &sink) { loop().post<Defer>(sink); }

static inline auto defer() {
    struct Await : public Sink {
        Coro<> _coro;

        Await() : Sink{} {}

        constexpr bool await_ready() const { return false; }

        void await_suspend(Coro<> coro) {
            _coro = coro;
            defer(*this);
        }

        void await_resume() const {}

        Res<> post(Async::Event &) override {
            _coro.resume();
            return Ok();
        }
    };

    return Await{};
}

/* --- Timer ---------------------------------------------------------------- */

struct Timer : public Source {
    struct Tick : public BaseEvent<Tick> {};

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

        if (Op::lt(_next, loop().now())) {
            loop().post<Tick>(sink);

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

static inline auto after(TimeStamp stamp) {
    struct Await : public Sink {
        TimeStamp _stamp;
        Opt<Timer> _timer;
        Coro<> _coro = nullptr;

        Await(TimeStamp stamp)
            : Sink{}, _stamp{stamp} {}

        constexpr bool await_ready() const {
            return false;
        }

        void await_suspend(Coro<> coro) {
            _coro = coro;
            _timer.emplace(*this, _stamp, NONE);
        }

        void await_resume() const {
        }

        Res<> post(Async::Event &) override {
            if (_coro) {
                auto coro = std::exchange(_coro, nullptr);
                coro.resume();
            } else
                panic("no coro");
            return Ok();
        }
    };

    return Await{stamp};
}

static inline auto after(TimeSpan span) {
    return after(loop().now() + span);
}

} // namespace Karm::Async
