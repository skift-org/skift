#include <fcntl.h>
#include <karm-embed/sys.h>
#include <unistd.h>

#include "utils.h"

namespace Karm::Embed {

struct PosixFd : public Sys::Fd {
    int _raw;

    Base::Result<size_t> read(void *buf, size_t size) override {
        ssize_t result = ::read(_raw, buf, size);

        if (result < 0) {
            return Posix::fromLastErrno();
        }

        return result;
    }

    Base::Result<size_t> write(void const *buf, size_t size) override {
        ssize_t result = ::write(_raw, buf, size);

        if (result < 0) {
            return Posix::fromLastErrno();
        }

        return result;
    }

    Base::Result<size_t> seek(Io::Seek seek) override {
        off_t offset = 0;

        switch (seek.whence) {
        case Io::Whence::BEGIN:
            offset = 0;
            break;
        case Io::Whence::CURRENT:
            offset = lseek(_raw, seek.offset, SEEK_CUR);
            break;
        case Io::Whence::END:
            offset = lseek(_raw, seek.offset, SEEK_END);
            break;
        }

        if (offset < 0) {
            return Posix::fromLastErrno();
        }

        return offset;
    }

    Base::Result<size_t> flush() override {
        /* NOTE: No-op */
        return 0;
    }

    Base::Result<Base::Strong<Fd>> dup() override {
        int duped = ::dup(_raw);

        if (duped < 0) {
            return Posix::fromLastErrno();
        }

        return {Base::makeStrong<PosixFd>(duped)};
    }
};

Base::Result<Base::Strong<Sys::Fd>> openFile(Sys::Path path) {
    int fd = ::open(path.c_str(), O_RDONLY);

    if (fd < 0) {
        return Posix::fromLastErrno();
    }

    return {Base::makeStrong<PosixFd>(fd)};
}

Base::Result<Base::Strong<Sys::Fd>> createFile(Sys::Path path) {
    int fd = ::open(path.c_str(), O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return Posix::fromLastErrno();
    }

    return {Base::makeStrong<PosixFd>(fd)};
}

Base::Result<Base::Cons<Base::Strong<Sys::Fd>, Base::Strong<Sys::Fd>>> createPipe() {
    int fds[2];

    if (::pipe(fds) < 0) {
        return Posix::fromLastErrno();
    }

    return {
        Base::makeStrong<PosixFd>(fds[0]),
        Base::makeStrong<PosixFd>(fds[1])};
}

Base::Result<Base::Strong<Sys::Fd>> createIn() {
    return Base::makeStrong<PosixFd>(0);
}

Base::Result<Base::Strong<Sys::Fd>> createOut() {
    return Base::makeStrong<PosixFd>(1);
}

Base::Result<Base::Strong<Sys::Fd>> createErr() {
    return Base::makeStrong<PosixFd>(2);
}

} // namespace Karm::Embed
