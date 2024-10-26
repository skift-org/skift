#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//
#include <impl-posix/fd.h>
#include <impl-posix/utils.h>
#include <karm-base/map.h>
#include <karm-sys/async.h>
#include <karm-sys/time.h>

#include <karm-sys/_embed.h>

namespace Karm::Sys::_Embed {

struct DarwinSched :
    public Sys::Sched {

    int _kqueue;
    usize _id = 0;
    Map<usize, Async::Promise<>> _promises;

    DarwinSched(int kqueue)
        : _kqueue(kqueue) {
    }

    ~DarwinSched() {
        close(_kqueue);
    }

    struct timespec _computeTimeout(TimeStamp until) {
        TimeStamp now = Sys::now();
        TimeSpan delta = TimeSpan::zero();
        if (now < until)
            delta = until - now;
        return Posix::toTimespec(delta);
    }

    Async::Task<> waitFor(struct kevent64_s ev) {
        usize id = _id++;
        auto promise = Async::Promise<>();
        auto future = promise.future();

        ev.udata = id;
        ::kevent64(
            _kqueue,

            &ev,
            1,

            nullptr,
            0,

            0,
            nullptr
        );

        _promises.put(id, std::move(promise));
        return Async::makeTask(future);
    }

    Async::Task<usize> readAsync(Strong<Fd> fd, MutBytes buf) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_READ,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->read(buf)));
    }

    Async::Task<usize> writeAsync(Strong<Fd> fd, Bytes buf) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_WRITE,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->write(buf)));
    }

    Async::Task<usize> flushAsync(Strong<Fd> fd) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_WRITE,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->flush()));
    }

    Async::Task<_Accepted> acceptAsync(Strong<Fd> fd) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_READ,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->accept()));
    }

    Async::Task<_Sent> sendAsync(Strong<Fd> fd, Bytes buf, Slice<Handle> handles, SocketAddr addr) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_WRITE,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->send(buf, handles, addr)));
    }

    Async::Task<_Received> recvAsync(Strong<Fd> fd, MutBytes buf, MutSlice<Handle> hnds) override {
        co_trya$(waitFor({
            .ident = fd->handle().value(),
            .filter = EVFILT_READ,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = 0,
            .data = 0,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok(co_try$(fd->recv(buf, hnds)));
    }

    Async::Task<> sleepAsync(TimeStamp until) override {
        struct timespec ts = _computeTimeout(until);

        co_trya$(waitFor({
            .ident = 0,
            .filter = EVFILT_TIMER,
            .flags = EV_ADD | EV_ONESHOT,
            .fflags = NOTE_NSECONDS,
            .data = ts.tv_sec * 1'000'000'000 + ts.tv_nsec,
            .udata = 0,
            .ext = {},
        }));
        co_return Ok();
    }

    Res<> wait(TimeStamp until) override {
        struct kevent64_s ev;
        struct timespec ts = _computeTimeout(until);

        int n = ::kevent64(
            _kqueue,

            nullptr,
            0,

            &ev,
            1,

            0,
            until.isEndOfTime() ? nullptr : &ts
        );

        if (n < 0)
            return Posix::fromLastErrno();

        if (n == 0)
            return Ok();

        usize id = ev.udata;
        auto promise = _promises.take(id);
        promise.resolve(Ok());
        return Ok();
    }
};

Sched &globalSched() {
    static DarwinSched sched = [] {
        int kqueue = ::kqueue();
        if (kqueue < 0)
            panic("kqueue");
        return DarwinSched(kqueue);
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
