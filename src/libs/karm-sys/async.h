#pragma once

#include <karm-base/box.h>
#include <karm-base/map.h>

#include "fd.h"

namespace Karm::Sys {

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

/* --- Scheduler ------------------------------------------------------------ */

struct Sink;
struct Source;

template <typename T = None, typename E = Error>
struct Task;

struct Sched :
    public Meta::Static {
    struct Queued {
        Sink *sink;
        Box<Event> event;
    };

    struct Binding {
        Source *source;
        Sink *sink;
    };

    Opt<Res<>> _ret;
    Vec<Queued> _queued;
    Vec<Binding> _sources;

    virtual ~Sched() = default;

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

    /* --- Operations --- */

    virtual Task<usize> readAsync(Strong<Fd> fd, MutBytes buf) = 0;

    virtual Task<usize> writeAsync(Strong<Fd> fd, Bytes buf) = 0;

    virtual Task<usize> flushAsync(Strong<Fd> fd) = 0;

    virtual Task<Accepted> acceptAsync(Strong<Fd> fd) = 0;

    virtual Task<usize> sendAsync(Strong<Fd> fd, Bytes buf, SocketAddr addr) = 0;

    virtual Task<Received> recvAsync(Strong<Fd> fd, MutBytes buf) = 0;

    virtual Task<> sleepAsync(TimeStamp until) = 0;
};

Sched &globalSched();

/* --- Sink ----------------------------------------------------------------- */

struct Sink : Meta::NoCopy {
    Sched &_sched;

    Sink(Sched &sched = globalSched())
        : _sched{sched} {}

    Sink(Sink &&other) : _sched{other._sched} {
        _sched._move(other, this);
    }

    Sink &operator=(Sink &&other) {
        _sched._move(other, this);
        return *this;
    }

    virtual ~Sink() {
        _sched._move(*this, nullptr);
    }

    // This needs to be protected to avoid Source from posting Event to a Sink directly.
protected:
    virtual Res<> post(Event &) = 0;
    friend struct Sched;
};

/* --- Source --------------------------------------------------------------- */

struct Source : Meta::NoCopy {
    Sched &_sched;

    Sched &sched() const { return _sched; }

    Source(Sched &sched = globalSched())
        : _sched{sched} {
    }

    Source(Source &&other) : _sched{other._sched} {
        _sched._move(other, this);
    }

    Source &operator=(Source &&other) {
        _sched._move(other, this);
        return *this;
    }

    void bind(Sink &sink) {
        _sched._bind(*this, sink);
    }

    virtual ~Source() {
        _sched._move(*this, nullptr);
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
    Awaiter(Sched &sched, T &&args)
        : Sink{sched}, _source{std::forward<T>(args)} {
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

    Res<> post(Event &e) override {
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

/* --- Task --------------------------------------------------------------- */

template <typename T = None, typename E = Error>
struct Promise {
    Opt<Res<T, E>> _value;

    void resolve(T value) {
        if (_value.has())
            panic("already resolved");
        _value.emplace(Ok(std::move(value)));
    }

    void reject(E error) {
        if (_value.has())
            panic("already resolved");
        _value.emplace(std::move(error));
    }

    void ret(Res<T, E> res) {
        if (_value.has())
            panic("already resolved");
        _value.emplace(std::move(res));
    }
};

template <typename T = None, typename E = Error>
Strong<Promise<T, E>> makePromise() {
    return makeStrong<Promise<T, E>>();
}

template <typename T, typename E>
struct [[nodiscard]] Task : public Source {
    using Inner = Var<Ok<T>, E>;

    struct _CoroPromise {
        Strong<Promise<T, E>> _hnd;

        // This is needed because by default the compiler will try to pass
        // all the coroutine arguments to the constructor of the promise
        _CoroPromise() : _hnd(makeStrong<Promise<T, E>>()) {}

        Task get_return_object() {
            return {_hnd, CoroHandle<_CoroPromise>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        void unhandled_exception() {}

        void return_value(Res<T, E> res) {
            _hnd->ret(std::move(res));
        }
    };

    struct Event {
        Res<T, E> res;
        Res<T, E> take() { return std::move(res); }
    };

    Strong<Promise<T, E>> _prom;
    CoroHandle<_CoroPromise> _coro;

    Task(Strong<Promise<T, E>> hnd, CoroHandle<_CoroPromise> coro = nullptr)
        : _prom(hnd), _coro(coro) {}

    Task(Task &&other)
        : _prom(std::move(other._prom)), _coro(std::exchange(other._coro, nullptr)) {
    }

    ~Task() {
        if (_coro)
            _coro.destroy();
    }

    virtual Res<TimeStamp> poll(Sink &sink) override {
        if (_prom->_value.has())
            _sched.post<Event>(sink, _prom->_value.take());
        return Ok(TimeStamp::endOfTime());
    }

    using promise_type = _CoroPromise;

    Awaiter<Task> operator co_await() {
        return {sched(), std::move(*this)};
    }
};

template <typename... Args>
inline Task<Tuple<SourceRet<Args>...>> waitAll(Args &...tasks) {
    co_return Ok<Tuple<SourceRet<Args>...>>({co_await tasks...});
}

template <typename T = Ok<>, typename E = Error>
void runDetached(Task<T, E> task) {
    struct Sink : public Sys::Sink {
        Task<T, E> _task;

        Sink(Task<T, E> task) : _task{std::move(task)} {
            task.bind(*this);
        }

        Res<> post(Sys::Event &) override {
            delete this;
            return Ok();
        }
    };
    new Sink{std::move(task)};
}

template <typename T = Ok<>, typename E = Error>
Res<T, E> runSync(Task<T, E> task, Sched &sched = globalSched()) {
    struct Sink : public Sys::Sink {
        Opt<Res<T, E>> &res;
        Sink(Opt<Res<T, E>> &res) : res{res} {}

        Res<> post(Sys::Event &e) override {
            if (auto *se = e.is<typename Task<T, E>::Event>()) {
                res = se->take();
                return Ok();
            }
            panic("unexpected event");
        }
    };

    Opt<Res<T, E>> res;
    Sink sink{res};
    task.bind(sink);
    while (true) {
        auto until = try$(sched.runOnce());
        if (res.has())
            return res.take();

        try$(sched.wait(until));
    }
}

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
            _sched.post<Event>(sink);
            _once = true;
        }
        return Ok(TimeStamp::endOfTime());
    };

    Awaiter<Defer> operator co_await() {
        return {sched(), std::move(*this)};
    }
};

static inline auto defer(Sched &sched = globalSched()) {
    return Defer{sched};
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

    Timer(TimeStamp next, Opt<TimeSpan> repeat = NONE, Sched &sched = globalSched())
        : Source(sched),
          _next{next},
          _repeat{repeat} {}

    Res<TimeStamp> poll(Sink &sink) override {
        if (not _active)
            return Ok(TimeStamp::endOfTime());

        auto now = _sched.now();
        if (_next < now) {
            _sched.post<Event>(sink, now);

            if (not _repeat) {
                _active = false;
                return Ok(TimeStamp::endOfTime());
            }

            _next += *_repeat;
        }

        return Ok(_next);
    };

    Awaiter<Timer> operator co_await() {
        return {sched(), std::move(*this)};
    }
};

static inline Timer every(TimeSpan span, Sched &sched = globalSched()) {
    return {sched.now() + span, span, sched};
}

static inline Timer after(TimeSpan span, Sched &sched = globalSched()) {
    return {sched.now() + span, NONE, sched};
}

static inline Timer after(TimeStamp stamp, Sched &sched = globalSched()) {
    return {stamp, NONE, sched};
}
} // namespace Karm::Sys
