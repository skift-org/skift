#include "fd.h"

#include "utils.h"

namespace Posix {

Fd::Fd(isize raw) : _raw(raw) {}

Fd::~Fd() {
    ::close(_raw);
}

usize Fd::handle() const {
    return static_cast<usize>(_raw);
}

Res<usize> Fd::read(MutBytes bytes) {
    isize result = ::read(_raw, bytes.buf(), sizeOf(bytes));

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok(static_cast<usize>(result));
}

Res<usize> Fd::write(Bytes bytes) {
    isize result = ::write(_raw, bytes.buf(), sizeOf(bytes));

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok(static_cast<usize>(result));
}

Res<usize> Fd::seek(Io::Seek seek) {
    off_t offset = 0;

    switch (seek.whence) {
    case Io::Whence::BEGIN:
        offset = lseek(_raw, seek.offset, SEEK_SET);
        break;
    case Io::Whence::CURRENT:
        offset = lseek(_raw, seek.offset, SEEK_CUR);
        break;
    case Io::Whence::END:
        offset = lseek(_raw, seek.offset, SEEK_END);
        break;
    }

    if (offset < 0)
        return Posix::fromLastErrno();

    return Ok(static_cast<usize>(offset));
}

Res<usize> Fd::flush() {
    // NOTE: No-op
    return Ok(0uz);
}

Res<Strong<Sys::Fd>> Fd::dup() {
    isize duped = ::dup(_raw);

    if (duped < 0)
        return Posix::fromLastErrno();

    return Ok(makeStrong<Fd>(duped));
}

Res<Sys::Accepted> Fd::accept() {
    struct sockaddr_in addr_;
    socklen_t len = sizeof(addr_);
    isize fd = ::accept(_raw, (struct sockaddr *)&addr_, &len);
    if (fd < 0)
        return Posix::fromLastErrno();

    return Ok<Sys::Accepted>(
        makeStrong<Fd>(fd),
        Posix::fromSockAddr(addr_));
}

Res<Sys::Stat> Fd::stat() {
    struct stat buf;
    if (fstat(_raw, &buf) < 0)
        return Posix::fromLastErrno();
    return Ok(Posix::fromStat(buf));
}

Res<> Fd::sendFd(Strong<Sys::Fd> fd) {
    auto *posixFd = fd.is<Fd>();
    if (not posixFd)
        return Error::invalidHandle("fd is not a posix fd");

    struct iovec iov {};
    // We need to send at least one byte of data
    char data{};
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);

    struct msghdr msg {};
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_controllen = CMSG_SPACE(sizeof(int));
    Array<Byte, CMSG_SPACE(sizeof(int))> ctrl_buf{};
    msg.msg_control = ctrl_buf.buf();

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *((int *)CMSG_DATA(cmsg)) = posixFd->_raw;

    if (::sendmsg(_raw, &msg, 0) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<Strong<Sys::Fd>> Fd::recvFd() {
    struct iovec iov {};
    // We need to send at least one byte of data
    char data{};
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);

    struct msghdr msg {};
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_controllen = CMSG_SPACE(sizeof(int));
    Array<Byte, CMSG_SPACE(sizeof(int))> ctrl_buf{};
    msg.msg_control = ctrl_buf.buf();

    if (::recvmsg(_raw, &msg, 0) < 0)
        return Posix::fromLastErrno();

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (not cmsg)
        return Error::invalidHandle("no cmsg");

    if (cmsg->cmsg_level != SOL_SOCKET)
        return Error::invalidHandle("invalid cmsg level");

    if (cmsg->cmsg_type != SCM_RIGHTS)
        return Error::invalidHandle("invalid cmsg type");

    if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
        return Error::invalidHandle("invalid cmsg len");

    int fd = *((int *)CMSG_DATA(cmsg));

    return Ok(makeStrong<Fd>(fd));
}

Res<usize> Fd::sendTo(Bytes bytes, Sys::SocketAddr addr) {
    struct sockaddr_in addr_ = Posix::toSockAddr(addr);
    isize result = ::sendto(_raw, bytes.buf(), sizeOf(bytes), 0, (struct sockaddr *)&addr_, sizeof(addr_));

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok(static_cast<usize>(result));
}

Res<Cons<usize, Sys::SocketAddr>> Fd::recvFrom(MutBytes bytes) {
    struct sockaddr_in addr_;
    socklen_t len = sizeof(addr_);
    isize result = ::recvfrom(_raw, bytes.buf(), sizeOf(bytes), 0, (struct sockaddr *)&addr_, &len);

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok<Cons<usize, Sys::SocketAddr>>(
        static_cast<usize>(result),
        Posix::fromSockAddr(addr_));
}

} // namespace Posix
