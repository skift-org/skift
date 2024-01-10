
#include <liburing.h>

//
#include <impl-posix/fd.h>
#include <impl-posix/utils.h>
#include <karm-logger/logger.h>
#include <karm-sys/async.h>
#include <karm-sys/time.h>

#include <karm-sys/_embed.h>

namespace Karm::Sys::_Embed {

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
    TimeStamp _now;

    UringSched(io_uring ring)
        : _ring(ring), _now(Sys::now()) {}

    ~UringSched() {
        io_uring_queue_exit(&_ring);
    }

    auto submit(Strong<_Job> job) {
        auto id = _id++;
        auto *sqe = io_uring_get_sqe(&_ring);
        if (not sqe)
            panic("failed to get sqe");
        sqe->user_data = id;
        _jobs.put(id, job);
        job->submit(sqe);
        io_uring_submit(&_ring);
    }

    Task<usize> readAsync(Strong<Fd> fd, MutBytes buf) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            MutBytes _buf;
            Strong<Promise<usize>> _promise = makePromise<usize>();

            Job(Strong<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_read(sqe, _fd->handle(), _buf.buf(), _buf.len(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve(cqe->res);
            }
        };

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return job->_promise;
    }

    Task<usize> writeAsync(Strong<Fd> fd, Bytes buf) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            Bytes _buf;
            Strong<Promise<usize>> _promise = makePromise<usize>();

            Job(Strong<Fd> fd, Bytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_write(sqe, _fd->handle(), _buf.buf(), _buf.len(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve(cqe->res);
            }
        };

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return job->_promise;
    }

    Task<usize> flushAsync(Strong<Fd> fd) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            Strong<Promise<usize>> _promise = makePromise<usize>();

            Job(Strong<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_fsync(sqe, _fd->handle(), 0);
            }

            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve(cqe->res);
            }
        };

        auto job = makeStrong<Job>(fd);
        submit(job);
        return job->_promise;
    }

    Task<Accepted> acceptAsync(Strong<Fd> fd) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            sockaddr_in _addr{};
            unsigned _addrLen = sizeof(sockaddr_in);
            Strong<Promise<Accepted>> _promise = makePromise<Accepted>();

            Job(Strong<Fd> fd)
                : _fd(fd) {}

            void submit(io_uring_sqe *sqe) override {
                io_uring_prep_accept(sqe, _fd->handle(), (struct sockaddr *)&_addr, &_addrLen, 0);
            }

            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve({makeStrong<Posix::Fd>(res), Posix::fromSockAddr(_addr)});
            }
        };

        auto job = makeStrong<Job>(fd);
        submit(job);
        return job->_promise;
    }

    Task<usize> sendAsync(Strong<Fd> fd, Bytes buf, SocketAddr addr) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            Bytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;
            Strong<Promise<usize>> _promise = makePromise<usize>();

            Job(Strong<Fd> fd, Bytes buf, SocketAddr addr)
                : _fd(fd), _buf(buf), _addr(Posix::toSockAddr(addr)) {}

            void submit(io_uring_sqe *sqe) override {
                _iov.iov_base = const_cast<Byte *>(_buf.begin());
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_sendmsg(sqe, _fd->handle(), &_msg, 0);
            }
            void complete(io_uring_cqe *cqe) override {
                auto res = cqe->res;
                if (res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve(cqe->res);
            }
        };

        auto job = makeStrong<Job>(fd, buf, addr);
        submit(job);
        return job->_promise;
    }

    Task<Received> recvAsync(Strong<Fd> fd, MutBytes buf) override {
        struct Job : public _Job {
            Strong<Fd> _fd;
            MutBytes _buf;
            iovec _iov;
            msghdr _msg;
            sockaddr_in _addr;

            Strong<Promise<Received>> _promise = makePromise<Received>();

            Job(Strong<Fd> fd, MutBytes buf)
                : _fd(fd), _buf(buf) {}

            void submit(io_uring_sqe *sqe) override {
                _iov.iov_base = _buf.begin();
                _iov.iov_len = _buf.len();

                _msg.msg_name = &_addr;
                _msg.msg_namelen = sizeof(sockaddr_in);
                _msg.msg_iov = &_iov;
                _msg.msg_iovlen = 1;

                io_uring_prep_recvmsg(sqe, _fd->handle(), &_msg, 0);
            }
            void complete(io_uring_cqe *cqe) override {
                if (cqe->res < 0) {
                    _promise->reject(Posix::fromLastErrno());
                    return;
                }
                _promise->resolve({(usize)cqe->res, Posix::fromSockAddr(_addr)});
            }
        };

        auto job = makeStrong<Job>(fd, buf);
        submit(job);
        return job->_promise;
    }

    Task<> sleepAsync(TimeStamp until) override {
        struct Job : public _Job {
            TimeStamp _until;
            Strong<Promise<>> _promise = makePromise<>();

            Job(TimeStamp until)
                : _until(until) {}

            void submit(io_uring_sqe *sqe) override {
                auto timeout = _until - Sys::now();
                struct __kernel_timespec ts;
                ts.tv_sec = timeout.toSecs();
                ts.tv_nsec = timeout.toUSecs() % 1000000000;
                io_uring_prep_timeout(sqe, &ts, 0, 0);
            }
            void complete(io_uring_cqe *cqe) override {
                if (cqe->res < 0)
                    _promise->reject(Posix::fromLastErrno());
                else
                    _promise->resolve(NONE);
            }
        };

        auto job = makeStrong<Job>(until);
        submit(job);
        return job->_promise;
    }

    TimeStamp now() override {
        return _now;
    }

    Res<> wait(TimeStamp until) override {
        auto timeout = until - Sys::now();
        struct __kernel_timespec ts;
        ts.tv_sec = timeout.toSecs();
        ts.tv_nsec = timeout.toUSecs() % 1000000000;

        Array<io_uring_cqe *, NCQES> cqes{};

        auto res = io_uring_wait_cqes(&_ring, cqes.buf(), 1, &ts, nullptr);

        _now = Sys::now();
        if (res < 0)
            return Posix::fromLastErrno();

        for (auto *cqe : cqes) {
            if (not cqe)
                break;
            auto id = cqe->user_data;
            auto job = _jobs.get(id).unwrap("invalid job id");
            job->complete(cqe);
            _jobs.del(id);
            io_uring_cqe_seen(&_ring, cqe);
        }
        return Ok();
    }
};

Sched &globalSched() {
    static UringSched sched = []() {
        io_uring ring{};
        auto res = io_uring_queue_init(UringSched::NCQES, &ring, 0);
        if (res < 0)
            panic("failed to initialize io_uring");
        return UringSched(ring);
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
