#include <unistd.h>

#include "fd.h"
#include "utils.h"

namespace Karm::Posix {

Fd::Fd(isize raw) : _raw(raw) {}

Fd::~Fd() {
    if (not _leak)
        ::close(_raw);
}

Sys::Handle Fd::handle() const {
    return Sys::Handle{static_cast<usize>(_raw)};
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

Res<> Fd::flush() {
    // NOTE: No-op
    return Ok();
}

Res<Rc<Sys::Fd>> Fd::dup() {
    isize duped = ::dup(_raw);

    if (duped < 0)
        return Posix::fromLastErrno();

    return Ok(makeRc<Fd>(duped));
}

Res<Sys::_Accepted> Fd::accept() {
    struct sockaddr_in addr_;
    socklen_t len = sizeof(addr_);
    isize fd = ::accept(_raw, (struct sockaddr*)&addr_, &len);
    if (fd < 0)
        return Posix::fromLastErrno();

    return Ok<Sys::_Accepted>(
        makeRc<Fd>(fd),
        Posix::fromSockAddr(addr_)
    );
}

Res<Sys::Stat> Fd::stat() {
    struct stat buf;
    if (fstat(_raw, &buf) < 0)
        return Posix::fromLastErrno();
    return Ok(Posix::fromStat(buf));
}

Res<Sys::_Sent> Fd::send(Bytes bytes, Slice<Sys::Handle> hnds, Sys::SocketAddr addr) {
    if (hnds.len() > 0)
        // TODO: Implement handle passing on POSIX
        notImplemented();

    struct sockaddr_in addr_ = Posix::toSockAddr(addr);
    isize result = ::sendto(_raw, bytes.buf(), sizeOf(bytes), 0, (struct sockaddr*)&addr_, sizeof(addr_));

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok<Sys::_Sent>(static_cast<usize>(result), 0);
}

Res<Sys::_Received> Fd::recv(MutBytes bytes, MutSlice<Sys::Handle>) {
    struct sockaddr_in addr_;
    socklen_t len = sizeof(addr_);
    isize result = ::recvfrom(_raw, bytes.buf(), sizeOf(bytes), 0, (struct sockaddr*)&addr_, &len);

    if (result < 0)
        return Posix::fromLastErrno();

    return Ok<Sys::_Received>(
        static_cast<usize>(result),
        0,
        Posix::fromSockAddr(addr_)
    );
}

Res<> Fd::pack(Sys::MessageWriter& e) {
    e.give(handle());
    return Ok();
}

} // namespace Karm::Posix
