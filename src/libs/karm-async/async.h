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

#include "event.h"

namespace Karm::Async {

/* --- Loop ----------------------------------------------------------------- */

struct Sink;
struct Source;

template <typename T>
struct Task;

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

    bool exited() const {
        return _ret.has() or _sources.len() == 0;
    }

    Res<> takeResult() {
        if (_ret.has())
            return _ret.take();
        return Ok();
    }

    void _collect();

    Res<TimeStamp> poll();

    Res<usize> dispatch();

    virtual TimeStamp now() = 0;

    virtual Res<> wait(TimeStamp until) = 0;

    Res<> run();

    Res<TimeStamp> runOnce();

    void quit(Res<> ret) { _ret = ret; }

    template <typename E, typename... Args>
    void post(Sink &sink, Args &&...args) {
        _post(sink, makeEvent<E>(Propagation::NONE, std::forward<Args>(args)...));
    }
};

static inline Loop &globalLoop() { return _Embed::loop(); }

/* --- Sink ----------------------------------------------------------------- */

struct Sink : public Meta::NoCopy {
    Loop &_loop;

    Sink(Loop &loop = globalLoop())
        : _loop{loop} {}

    Sink(Sink &&other) : _loop{other._loop} {
        _loop._move(other, this);
    }

    Sink &operator=(Sink &&other) {
        _loop._move(other, this);
        return *this;
    }

    virtual ~Sink() {
        _loop._move(*this, nullptr);
    }

    // This needs to be protected to avoid Source from posting Event to a Sink directly.
protected:
    virtual Res<> post(Event &) = 0;
    friend struct Loop;
};

/* --- Source --------------------------------------------------------------- */

struct Source : public Meta::NoCopy {
    Loop &_loop;

    Loop &loop() const { return _loop; }

    Source(Loop &loop = globalLoop())
        : _loop{loop} {
    }

    Source(Source &&other) : _loop{other._loop} {
        _loop._move(other, this);
    }

    Source &operator=(Source &&other) {
        _loop._move(other, this);
        return *this;
    }

    void bind(Sink &sink) {
        _loop._bind(*this, sink);
    }

    virtual ~Source() {
        _loop._move(*this, nullptr);
    }

    virtual Res<TimeStamp> poll(Sink &) = 0;
};

template <typename T>
struct _SourceRet;

template <Takeable T>
struct _SourceRet<T> {
    using Type = TakeableType<T>;
};

template <typename T>
    requires(not Takeable<T>)
struct _SourceRet<T> {
    using Type = Ok<>;
};

template <typename T>
using SourceRet = typename _SourceRet<typename T::Event>::Type;

template <typename T>
using EventRet = typename _SourceRet<T>::Type;

/* --- Awaitable ------------------------------------------------------------ */

template <typename T = void>
using CoroHandle = std::coroutine_handle<T>;

template <typename Source>
struct Awaiter : public Sink {
    Source _source;
    CoroHandle<> _coro = nullptr;
    Opt<SourceRet<Source>> _ret;

    template <typename T>
    Awaiter(Loop &loop, T &&args)
        : Sink{loop}, _source{std::forward<T>(args)} {
        _source.bind(*this);
    }

    constexpr bool await_ready() const {
        return false;
    }

    void await_suspend(CoroHandle<> coro) {
        _coro = coro;
    }

    SourceRet<Source> await_resume() {
        return _ret.take();
    }

    Res<> post(Async::Event &e) override {
        if (not _coro)
            panic("no coro");

        if (_ret.has())
            panic("already resumed");

        if (auto *se = e.is<typename Source::Event>()) {
            if constexpr (Takeable<typename Source::Event>) {
                _ret = se->take();
            } else {
                _ret = Ok();
            }

            auto coro = std::exchange(_coro, nullptr);
            coro.resume();
            return Ok();
        }

        panic("unexpected event");
    }
};

/* --- Task ----------------------------------------------------------------- */

template <typename T = Ok<>>
struct [[nodiscard]] Task :
    public Source {
    using Inner = T;

    struct Event {
        T res;
        T take() { return std::move(res); }
    };

    struct Promise {
        Opt<T> _ret = NONE;

        // This is needed because by default the compiler will try to pass
        // all the coroutine arguments to the constructor of the promise
        Promise() {}

        Task get_return_object() {
            return {CoroHandle<Promise>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() {
            return {};
        }
        std::suspend_always final_suspend() noexcept {
            return {};
        }

        void unhandled_exception() {}

        void return_value(T res) {
            if (_ret.has())
                panic("already returned");
            _ret = std::move(res);
        }

        bool has() const {
            return _ret.has();
        }

        T take() {
            return _ret.take();
        }
    };

    using promise_type = Promise;

    CoroHandle<Promise> _coro;

    Promise &promise() {
        return _coro.promise();
    }

    Task(CoroHandle<Promise> coro)
        : _coro(coro) {}

    Task(Task &&other)
        : _coro(std::exchange(other._coro, nullptr)) {}

    ~Task() {
        if (_coro)
            _coro.destroy();
    }

    Res<TimeStamp> poll(Sink &sink) override {
        if (not _coro)
            return Ok(TimeStamp::endOfTime());

        if (promise().has()) {
            _loop.post<Event>(sink, promise().take());
            _coro.destroy();
            _coro = nullptr;
            return Ok(TimeStamp::epoch());
        }

        return Ok(TimeStamp::endOfTime());
    }

    Awaiter<Task<T>> operator co_await() {
        return {loop(), std::move(*this)};
    }
};

template <typename... Args>
inline Task<Tuple<typename Args::Inner...>> all(Args &...tasks) {
    co_return {co_await tasks...};
}

template <typename T = Ok<>>
void runDetached(Task<T> task);

template <typename T = Ok<>>
Res<T> runSync(Task<T> task, Loop &loop = globalLoop()) {
    Opt<T> res;
    struct Sink : public Async::Sink {
        Opt<T> &res;
        Sink(Opt<T> &res) : res{res} {}

        Res<> post(Async::Event &e) override {
            if (auto *se = e.is<typename Task<T>::Event>()) {
                res = se->take();
                return Ok();
            }
            panic("unexpected event");
        }
    };

    Sink sink{res};
    task.bind(sink);
    while (true) {
        auto until = try$(loop.runOnce());
        if (res.has())
            return Ok(res.take());
        try$(loop.wait(until));
    }
}

template <typename T = None>
using Prom = Task<Res<T>>;

/* --- Defer ---------------------------------------------------------------- */

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
            _loop.post<Event>(sink);
            _once = true;
        }
        return Ok(TimeStamp::endOfTime());
    };

    Awaiter<Defer> operator co_await() {
        return {loop(), std::move(*this)};
    }
};

static inline auto defer(Loop &loop = globalLoop()) {
    return Defer{loop};
}

/* --- Timer ---------------------------------------------------------------- */

struct Timer : public Source {
    struct Event {
        TimeStamp now;
        TimeStamp take() { return now; }
    };

    TimeStamp _next;
    Opt<TimeSpan> _repeat;
    bool _active = true;

    Timer(TimeStamp next, Opt<TimeSpan> repeat = NONE, Loop &loop = globalLoop())
        : Source(loop),
          _next{next},
          _repeat{repeat} {}

    Res<TimeStamp> poll(Sink &sink) override {
        if (not _active)
            return Ok(TimeStamp::endOfTime());

        auto now = _loop.now();
        if (_next < now) {
            _loop.post<Event>(sink, now);

            if (not _repeat) {
                _active = false;
                return Ok(TimeStamp::endOfTime());
            }

            _next += *_repeat;
        }

        return Ok(_next);
    };

    Awaiter<Timer> operator co_await() {
        return {loop(), std::move(*this)};
    }
};

static inline Timer every(TimeSpan span, Loop &loop = globalLoop()) {
    return {loop.now() + span, span, loop};
}

static inline Timer after(TimeSpan span, Loop &loop = globalLoop()) {
    return {loop.now() + span, NONE, loop};
}

static inline Timer after(TimeStamp stamp, Loop &loop = globalLoop()) {
    return {stamp, NONE, loop};
}

} // namespace Karm::Async
