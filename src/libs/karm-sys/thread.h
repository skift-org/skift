#pragma once

#include <karm-base/cons.h>
#include <karm-base/func.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/ring.h>

namespace Karm::Sys {

template <typename T>
struct Channel {
    Lock _lock;
    Ring<T> _ring;

    Channel(usize capacity)
        : _ring(capacity) {}

    Res<T> recv() {
        LockScope guard(_lock);

        if (_ring.empty()) {
            return Error::WOULD_BLOCK;
        }

        return _ring.pop();
    }

    Res<> send(T value) {
        LockScope guard(_lock);

        if (_ring.full()) {
            return Error::wouldBlock();
        }

        _ring.push(value);
        return Ok();
    }
};

template <typename T>
struct Tx {
    using Type = T;

    Strong<Channel<T>> _channel;

    Res<void> send(T const &value);
};

template <typename T>
struct Rx {
    using Type = T;

    Strong<Channel<T>> _channel;

    Res<T> receive();
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
    Res<Thread> spawn(Func<void(void)>);

    void join();

    void detach();

    void exit();

    void sleep(isize ms);
};

} // namespace Karm::Sys
