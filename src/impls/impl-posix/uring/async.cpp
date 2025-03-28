
#include <liburing.h>
//
#include <impl-posix/fd.h>
#include <impl-posix/utils.h>
#include <karm-async/promise.h>
#include <karm-base/map.h>
#include <karm-logger/logger.h>
#include <karm-sys/_embed.h>
#include <karm-sys/async.h>
#include <karm-sys/time.h>

namespace Karm::Sys::_Embed {

struct __kernel_timespec toKernelTimespec(Instant ts) {
    struct __kernel_timespec kts;
    if (ts.isEndOfTime()) {
        kts.tv_sec = LONG_MAX;
        kts.tv_nsec = 0;
        return kts;
    } else {
        kts.tv_sec = ts.val() / 1000000;
        kts.tv_nsec = (ts.val() % 1000000) * 1000;
    }
    return kts;
}

struct __kernel_timespec toKernelTimespec(Duration ts) {
    struct __kernel_timespec kts;
    if (ts.isInfinite()) {
        kts.tv_sec = LONG_MAX;
        kts.tv_nsec = 0;
        return kts;
    } else {
        kts.tv_sec = ts.val() / 1000000;
        kts.tv_nsec = (ts.val() % 1000000) * 1000;
    }
    return kts;
}

struct UringSched : public Sys::Sched {
    static constexpr auto NCQES = 128;

    struct _Job {
        virtual ~_Job() = default;
        virtual void submit(io_uring_sqe* sqe) = 0;
        virtual void complete(io_uring_cqe* cqe) = 0;
    };

    io_uring _ring;
    usize _id = 1;
    Map<usize, Rc<_Job>> _jobs;

    UringSched(io_uring ring)
        : _ring(ring) {}

    ~UringSched() {
        io_uring_queue_exit(&_ring);
    }

    void submit(Rc<_Job> job) {
        auto id = _id++;
        auto* sqe = io_uring_get_sqe(&_ring);
        if (not sqe) [[unlikely]]
            panic("failed to get sqe");
        _jobs.put(id, job);
        job->submit(sqe);
        sqe->user_data = id;
        io_uring_submit(&_ring);
    }

    Async::Task<usize> readAsync(Rc<Fd> fd, MutBytes buf) override {
        struct Job : public _Job {
            Rc<Fd> _fd;
            MutBytes _buf;
            Async::Promise<usize> _promise;

            Job(Rc<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe* sqe) override {
                io_uring_prep_read(
                    sqe,
                    _fd->handle().value(),
                    _buf.buf(),
                    _buf.len(),
                    -1
                );
            }

            void complete(io_uring_cqe* cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok(cqe->res));
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<usize> writeAsync(Rc<Fd> fd, Bytes buf) override {
        struct Job : public _Job {
            Rc<Fd> _fd;
            Bytes _buf;
            Async::Promise<usize> _promise;

            Job(Rc<Fd> fd, Bytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe* sqe) override {
                io_uring_prep_write(
                    sqe,
                    _fd->handle().value(),
                    _buf.buf(),
                    _buf.len(),
                    // NOTE: On files that support seeking, if the offset is set
                    //       to -1, the write operation commences at the file
                    //       offset, and the file offset is incremented by
                    //       the number of bytes written. See io_uring_prep_write(3).
                    -1
                );
            }

            void complete(io_uring_cqe* cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok(cqe->res));
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<> flushAsync(Rc<Fd> fd) override {
        struct Job : public _Job {
            Rc<Fd> _fd;
            Async::Promise<> _promise;

            Job(Rc<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe* sqe) override {
                io_uring_prep_fsync(sqe, _fd->handle().value(), 0);
            }

            void complete(io_uring_cqe* cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok());
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Accepted> acceptAsync(Rc<Fd> fd) override {
        struct Job : public _Job {
            Rc<Fd> _fd;
            sockaddr_in _addr{};
            unsigned _addrLen = sizeof(sockaddr_in);
            Async::Promise<_Accepted> _promise;

            Job(Rc<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe* sqe) override {
                io_uring_prep_accept(sqe, _fd->handle().value(), (struct sockaddr*)&_addr, &_addrLen, 0);
            }

            void complete(io_uring_cqe* cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else {
                    _Accepted accepted = {makeRc<Posix::Fd>(res), Posix::fromSockAddr(_addr)};
                    _promise.resolve(Ok(accepted));
                }
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Sent> sendAsync(Rc<Fd> fd, Bytes buf, Slice<Handle> handles, SocketAddr addr) override {
        if (handles.len() > 0)
            notImplemented(); // TODO: Implement handle passing on POSIX

        struct Job : public _Job {
            Rc<Fd> _fd;
            Bytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;
            Async::Promise<_Sent> _promise;

            Job(Rc<Fd> fd, Bytes buf, SocketAddr addr)
                : _fd(fd), _buf(buf), _addr(Posix::toSockAddr(addr)) {}

            void submit(io_uring_sqe* sqe) override {
                _iov.iov_base = const_cast<Byte*>(_buf.begin());
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_sendmsg(sqe, _fd->handle().value(), &_msg, 0);
            }

            void complete(io_uring_cqe* cqe) override {
                if (cqe->res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok<_Sent>(cqe->res, 0));
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd, buf, addr);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Received> recvAsync(Rc<Fd> fd, MutBytes buf, MutSlice<Handle>) override {
        struct Job : public _Job {
            Rc<Fd> _fd;
            MutBytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;
            Async::Promise<_Received> _promise;

            Job(Rc<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe* sqe) override {
                _iov.iov_base = _buf.begin();
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_recvmsg(sqe, _fd->handle().value(), &_msg, 0);
            }

            void complete(io_uring_cqe* cqe) override {
                if (cqe->res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else {
                    _Received received = {(usize)cqe->res, 0, Posix::fromSockAddr(_addr)};
                    _promise.resolve(Ok(received));
                }
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<> sleepAsync(Instant until) override {
        struct Job : public _Job {
            Instant _until;
            Async::Promise<> _promise;

            struct __kernel_timespec _ts{};

            Job(Instant until)
                : _until(until) {}

            void submit(io_uring_sqe* sqe) override {
                _ts = toKernelTimespec(_until);
                io_uring_prep_timeout(sqe, &_ts, 0, IORING_TIMEOUT_ABS);
            }

            void complete(io_uring_cqe* cqe) override {
                if (cqe->res < 0 and cqe->res != -ETIME)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok());
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeRc<Job>(until);
        submit(job);
        return Async::makeTask(job->future());
    }

    bool _inWait = false;

    Res<> wait(Instant until) override {
        if (_inWait)
            panic("nested wait");
        _inWait = true;
        Defer defer = [&] {
            _inWait = false;
        };

        Instant now = Sys::instant();

        Duration delta = Duration::zero();
        if (now < until)
            delta = until - now;

        struct __kernel_timespec ts = toKernelTimespec(delta);
        io_uring_cqe* cqe = nullptr;
        io_uring_wait_cqe_timeout(&_ring, &cqe, &ts);

        unsigned head;
        usize i = 0;
        io_uring_for_each_cqe(&_ring, head, cqe) {
            auto id = cqe->user_data;
            auto job = _jobs.get(id);
            job->complete(cqe);
            _jobs.del(id);
            ++i;
        }

        io_uring_cq_advance(&_ring, i);

        return Ok();
    }
};

Sched& globalSched() {
    static UringSched sched = [] {
        io_uring ring{};
        auto res = io_uring_queue_init(UringSched::NCQES, &ring, 0);
        if (res < 0) [[unlikely]]
            panic("failed to initialize io_uring");
        return UringSched(ring);
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
