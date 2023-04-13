#pragma once

#include <karm-base/func.h>
#include <karm-base/list.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-meta/nocopy.h>
#include <karm-sys/proc.h>
#include <karm-sys/time.h>

namespace Karm::Async {

template <typename T = void>
using Coro = std::coroutine_handle<T>;

using Eval = Func<TimeStamp()>;

struct Sched {
    struct Queued {
        Coro<> coro;
        Eval eval;
    };

    List<Queued> _queue;

    static Sched &instance() {
        static Sched sched;
        return sched;
    }

    void queue(Coro<> coro, Eval eval) {
        _queue.emplaceBack(coro, std::move(eval));
    }

    TimeStamp schedule(TimeStamp now) {
        auto soon = TimeStamp::endOfTime();
        for (usize i = 0; i < _queue.len(); i++) {
            auto &queued = _queue.peekFront();
            auto evaled = queued.eval();
            if (Op::lteq(evaled, now)) {
                queued.coro.resume();
                _queue.popFront();
                return now;
            }

            if (Op::lt(evaled, soon)) {
                soon = evaled;
            }

            _queue.requeue();
        }

        return soon;
    }

    Res<> run() {
        while (_queue.len() > 0) {
            TimeStamp now = Sys::now();
            auto soon = schedule(now);
            while (Op::lteq(soon, now)) {
                now = Sys::now();
                soon = schedule(now);
            }

            if (_queue.len() > 0)
                try$(Sys::sleepUntil(soon));
        }

        return Ok();
    }
};

template <typename Task>
struct Promise {
    Opt<typename Task::Res> _res = NONE;
    Task get_return_object() { return {Coro<Promise>::from_promise(*this)}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
    void return_value(typename Task::Res res) { _res = res; }
};

template <typename _Res = Ok<>>
struct Task : public Meta::NoCopy {
    using Res = _Res;
    using PromiseType = Promise<Task>;
    using CoroType = Coro<PromiseType>;

    using promise_type = PromiseType;

    CoroType _coro;

    Task(Coro<Promise<Task>> coro)
        : _coro(coro) {}

    Task(Task &&other)
        : _coro(std::exchange(other._coro, {})) {}

    ~Task() {
        if (_coro) {
            _coro.destroy();
        }
    }

    /* --- Awaitable -------------------------------------------------------- */

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(Coro<> coro) const noexcept {
        Sched::instance().queue(coro, [&]() {
            return _coro.done() ? TimeStamp::epoch() : TimeStamp::endOfTime();
        });
    }

    Res await_resume() const noexcept {
        return _coro.promise()._res.unwrap();
    }

    /* --- Run -------------------------------------------------------------- */

    Karm::Res<Res> runSync() {
        while (!_coro.done()) {
            auto until = Sched::instance().schedule(Sys::now());
            try$(Sys::sleepUntil(until));
        }
        return Ok(_coro.promise()._res.take());
    }
};

inline auto sleep(TimeSpan span) {
    struct Awaitable {
        TimeStamp _t;

        constexpr bool await_ready() const noexcept { return false; }

        void await_suspend(Coro<> coro) const noexcept {
            Sched::instance().queue(coro, [t = _t]() {
                return t;
            });
        }

        void await_resume() const noexcept {}
    };

    return Awaitable{Sys::now() + span};
}

inline auto yield() {
    struct Awaitable {
        constexpr bool await_ready() const noexcept { return false; }

        void await_suspend(Coro<> coro) const noexcept {
            Sched::instance().queue(coro, []() {
                return TimeStamp::epoch();
            });
        }

        void await_resume() const noexcept {}
    };

    return Awaitable{};
}

template <typename... Args>
inline Task<Tuple<typename Args::Res...>> all(Args &...tasks) {
    co_return {co_await tasks...};
}

} // namespace Karm::Async
