#include <impl-posix/fd.h>
#include <impl-posix/utils.h>
#include <karm-async/promise.h>
#include <karm-base/map.h>
#include <karm-sys/_embed.h>
#include <karm-sys/async.h>
#include <karm-sys/time.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace Karm::Sys::_Embed {

struct EpollSched : public Sys::Sched {
    int _epollFd;
    usize _id = 0;
    Map<usize, Async::Promise<>> _promises;

    EpollSched(int epollFd)
        : _epollFd(epollFd) {}

    ~EpollSched() { close(_epollFd); }

    Async::Task<> waitFor(epoll_event ev, int fd) {
        usize id = _id++;
        auto promise = Async::Promise<>();
        auto future = promise.future();

        ev.data.u64 = id;
        if (::epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
            panic("epoll_ctl");

        _promises.put(id, std::move(promise));
        return Async::makeTask(future);
    }

    Async::Task<usize> readAsync(Rc<Fd> fd, MutBytes buf) override {
        co_trya$(waitFor({.events = EPOLLIN | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->read(buf)));
    }

    Async::Task<usize> writeAsync(Rc<Fd> fd, Bytes buf) override {
        co_trya$(waitFor({.events = EPOLLOUT | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->write(buf)));
    }

    Async::Task<usize> flushAsync(Rc<Fd> fd) override {
        co_trya$(waitFor({.events = EPOLLOUT | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->flush()));
    }

    Async::Task<_Accepted> acceptAsync(Rc<Fd> fd) override {
        co_trya$(waitFor({.events = EPOLLIN | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->accept()));
    }

    Async::Task<_Sent> sendAsync(Rc<Fd> fd, Bytes buf, Slice<Handle> handles, SocketAddr addr) override {
        co_trya$(waitFor({.events = EPOLLOUT | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->send(buf, handles, addr)));
    }

    Async::Task<_Received> recvAsync(Rc<Fd> fd, MutBytes buf, MutSlice<Handle> hnds) override {
        co_trya$(waitFor({.events = EPOLLIN | EPOLLET, .data = {}}, fd->handle().value()));
        co_return Ok(co_try$(fd->recv(buf, hnds)));
    }

    Async::Task<> sleepAsync(Instant until) override {
        Instant instant = Sys::instant();
        Duration delta = Duration::zero();
        if (instant < until)
            delta = until - instant;

        int timeFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        if (timeFd < 0)
            co_return Posix::fromLastErrno();
        Defer defer{[&] {
            close(timeFd);
        }};

        itimerspec spec{};
        spec.it_value = Posix::toTimespec(delta);
        if (timerfd_settime(timeFd, 0, &spec, nullptr) < 0)
            co_return Posix::fromLastErrno();

        co_trya$(waitFor({.events = EPOLLIN, .data = {}}, timeFd));

        co_return Ok();
    }

    Res<> wait(Instant until) override {
        epoll_event ev;
        auto instant = Sys::instant();
        Duration delta = Duration::zero();
        if (instant < until)
            delta = until - instant;
        int timeout = until.isEndOfTime() ? -1 : delta.toMSecs();

        int n = ::epoll_wait(_epollFd, &ev, 1, timeout);

        if (n < 0)
            return Posix::fromLastErrno();

        if (n == 0)
            return Ok();

        usize id = ev.data.u64;
        auto promise = _promises.take(id);
        promise.resolve(Ok());
        return Ok();
    }
};

Sched& globalSched() {
    static EpollSched sched = [] {
        int fd = ::epoll_create1(0);
        if (fd < 0)
            panic("epoll_create1");
        return EpollSched(fd);
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
