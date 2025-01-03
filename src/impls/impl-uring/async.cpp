
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

struct __kernel_timespec toKernelTimespec(TimeStamp ts) {
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

struct __kernel_timespec toKernelTimespec(TimeSpan ts) {
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
        virtual void submit(io_uring_sqe *sqe) = 0;
        virtual void complete(io_uring_cqe *cqe) = 0;
    };

    io_uring _ring;
    usize _id = 0;
    Map<usize, Strong<_Job>> _jobs;

    UringSched(io_uring ring)
        : _ring(ring) {}

    ~UringSched() {
        io_uring_queue_exit(&_ring);
    }

    void submit(Strong<_Job> job) {
        auto id = _id++;
        auto *sqe = io_uring_get_sqe(&_ring);
        if (not sqe) [[unlikely]]
            panic("failed to get sqe");
        sqe->user_data = id;
        _jobs.put(id, job);
        job->submit(sqe);
        io_uring_submit(&_ring);
    }

    Async::Task<usize> readAsync(Strong<Fd> fd, MutBytes buf) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            MutBytes _buf;
            Async::Promise<usize> _promise;

            Job(Strong<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_read(sqe, _fd->handle().value(), _buf.buf(), _buf.len(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
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

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<usize> writeAsync(Strong<Fd> fd, Bytes buf) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            Bytes _buf;
            Async::Promise<usize> _promise;

            Job(Strong<Fd> fd, Bytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_write(sqe, _fd->handle().value(), _buf.buf(), _buf.len(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
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

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<usize> flushAsync(Strong<Fd> fd) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            Async::Promise<usize> _promise;

            Job(Strong<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_fsync(sqe, _fd->handle().value(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
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

        auto job = makeStrong<Job>(fd);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Accepted> acceptAsync(Strong<Fd> fd) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            sockaddr_in _addr{};
            unsigned _addrLen = sizeof(sockaddr_in);
            Async::Promise<_Accepted> _promise;

            Job(Strong<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_accept(sqe, _fd->handle().value(), (struct sockaddr *)&_addr, &_addrLen, 0);
            }

            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else {
                    _Accepted accepted = {makeStrong<Posix::Fd>(res), Posix::fromSockAddr(_addr)};
                    _promise.resolve(Ok(accepted));
                }
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeStrong<Job>(fd);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Sent> sendAsync(Strong<Fd> fd, Bytes buf, Slice<Handle> handles, SocketAddr addr) override {
        if (handles.len() > 0)
            notImplemented(); // TODO: Implement handle passing on POSIX

        struct Job : public _Job {
            Strong<Fd> _fd;
            Bytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;
            Async::Promise<_Sent> _promise;

            Job(Strong<Fd> fd, Bytes buf, SocketAddr addr)
                : _fd(fd), _buf(buf), _addr(Posix::toSockAddr(addr)) {}

            void submit(io_uring_sqe *sqe) override {
                _iov.iov_base = const_cast<Byte *>(_buf.begin());
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_sendmsg(sqe, _fd->handle().value(), &_msg, 0);
            }

            void complete(io_uring_cqe *cqe) override {
                if (cqe->res < 0)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok<_Sent>(cqe->res, 0));
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeStrong<Job>(fd, buf, addr);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<_Received> recvAsync(Strong<Fd> fd, MutBytes buf, MutSlice<Handle>) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            MutBytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;
            Async::Promise<_Received> _promise;

            Job(Strong<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                _iov.iov_base = _buf.begin();
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_recvmsg(sqe, _fd->handle().value(), &_msg, 0);
            }

            void complete(io_uring_cqe *cqe) override {
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

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return Async::makeTask(job->future());
    }

    Async::Task<> sleepAsync(TimeStamp until) override {
        struct Job : public _Job {
            TimeStamp _until;
            Async::Promise<> _promise;

            struct __kernel_timespec _ts{};

            Job(TimeStamp until)
                : _until(until) {}

            void submit(io_uring_sqe *sqe) override {
                _ts = toKernelTimespec(_until);
                io_uring_prep_timeout(sqe, &_ts, 0, IORING_TIMEOUT_ABS | IORING_TIMEOUT_REALTIME);
            }

            void complete(io_uring_cqe *cqe) override {
                if (cqe->res < 0 and cqe->res != -ETIME)
                    _promise.resolve(Posix::fromErrno(-cqe->res));
                else
                    _promise.resolve(Ok());
            }

            auto future() {
                return _promise.future();
            }
        };

        auto job = makeStrong<Job>(until);
        submit(job);
        return Async::makeTask(job->future());
    }

    Res<> wait(TimeStamp until) override {
        // HACK: io_uring_wait_cqes doesn't support absolute timeout
        //       so we have to do it ourselves
        TimeStamp now = Sys::now();

        TimeSpan delta = TimeSpan::zero();
        if (now < until)
            delta = until - now;

        struct __kernel_timespec ts = toKernelTimespec(delta);
        Array<io_uring_cqe *, NCQES> cqes{};
        io_uring_wait_cqes(&_ring, cqes.buf(), 1, &ts, nullptr);

        for (auto *cqe : cqes) {
            if (not cqe)
                break;

            auto id = cqe->user_data;
            auto job = _jobs.get(id);
            job->complete(cqe);
            _jobs.del(id);
            io_uring_cqe_seen(&_ring, cqe);
        }
        return Ok();
    }
};

Sched &globalSched() {
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
