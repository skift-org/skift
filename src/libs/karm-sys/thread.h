#pragma once

#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/result.h>
#include <karm-base/ring.h>

namespace Karm::Sys {

template <typename T>
struct Channel {
    Lock _lock;
    Ring<T> _ring;

    Channel(size_t capacity)
        : _ring(capacity) {}

    Result<T> recv() {
        LockScope guard(_lock);

        if (_ring.empty()) {
            return Error::WOULD_BLOCK;
        }

        return _ring.pop();
    }

    Error send(T value) {
        LockScope guard(_lock);

        if (_ring.full()) {
            return Error::WOULD_BLOCK;
        }

        _ring.push(value);
        return OK;
    }
};

template <typename T>
struct Tx {
    using Type = T;

    Strong<Channel<T>> _channel;

    Result<void> send(T const &value);
};

template <typename T>
struct Rx {
    using Type = T;

    Strong<Channel<T>> _channel;

    Result<T> receive();
};

template <typename T>
Cons<Tx<T>, Rx<T>> makeChannel() {
    auto channel = makeStrong<Channel<T>>(128);

    return {
        Tx<T>{channel},
        Rx<T>{channel},
    };
}

struct Thread {
    Result<Thread> spawn(Func<void(void)>);

    void join();

    void detach();

    void exit();

    void sleep(int ms);
};

} // namespace Karm::Sys
