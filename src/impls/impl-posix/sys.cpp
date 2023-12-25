#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

//
#include <karm-io/funcs.h>
#include <karm-logger/logger.h>

#include <karm-sys/_embed.h>

#include "errno.h"

namespace Karm::Sys::_Embed {

/* --- Utilities ------------------------------------------------------------ */

static struct sockaddr_in toSockAddr(SocketAddr addr) {
    struct sockaddr_in sockaddr;
    auto addr4 = addr.addr.unwrap<Sys::Ip4>();
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(addr.port);
    sockaddr.sin_addr.s_addr = addr4._raw._value;
    return sockaddr;
}

static SocketAddr fromSockAddr(struct sockaddr_in sockaddr) {
    SocketAddr addr{Ip4::unspecified(), 0};
    addr.addr.unwrap<Sys::Ip4>()._raw._value = sockaddr.sin_addr.s_addr;
    addr.port = ntohs(sockaddr.sin_port);
    return addr;
}

static Stat fromStat(struct stat const &buf) {
    Stat stat{};
    Stat::Type type = Stat::FILE;
    if (S_ISDIR(buf.st_mode))
        type = Stat::DIR;
    stat.type = type;
    stat.size = (usize)buf.st_size;
    stat.accessTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_atime);
    stat.modifyTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_mtime);
    stat.changeTime = TimeStamp::epoch() + TimeSpan::fromSecs(buf.st_ctime);
    return stat;
}

/* --- File Descriptor ------------------------------------------------------ */

struct PosixFd : public Sys::Fd {
    isize _raw;

    PosixFd(isize raw) : _raw(raw) {}

    Res<usize> read(MutBytes bytes) override {
        isize result = ::read(_raw, bytes.buf(), sizeOf(bytes));

        if (result < 0)
            return Posix::fromLastErrno();

        return Ok(static_cast<usize>(result));
    }

    Res<usize> write(Bytes bytes) override {
        isize result = ::write(_raw, bytes.buf(), sizeOf(bytes));

        if (result < 0)
            return Posix::fromLastErrno();

        return Ok(static_cast<usize>(result));
    }

    Res<usize> seek(Io::Seek seek) override {
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

    Res<usize> flush() override {
        // NOTE: No-op
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        isize duped = ::dup(_raw);

        if (duped < 0)
            return Posix::fromLastErrno();

        return Ok(makeStrong<PosixFd>(duped));
    }

    Res<Cons<Strong<Fd>, SocketAddr>> accept() override {
        struct sockaddr_in addr_;
        socklen_t len = sizeof(addr_);
        isize fd = ::accept(_raw, (struct sockaddr *)&addr_, &len);
        if (fd < 0)
            return Posix::fromLastErrno();

        return Ok<Cons<Strong<Fd>, SocketAddr>>(
            makeStrong<PosixFd>(fd),
            fromSockAddr(addr_));
    }

    Res<Stat> stat() override {
        struct stat buf;
        if (fstat(_raw, &buf) < 0)
            return Posix::fromLastErrno();
        return Ok(fromStat(buf));
    }

    Res<> sendFd(Strong<Fd> fd) override {
        auto *posixFd = fd.is<PosixFd>();
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

    Res<Strong<Fd>> recvFd() override {
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

        return Ok(makeStrong<PosixFd>(fd));
    }
};

Res<Url::Path> resolve(Url::Url const &url) {
    Url::Path resolved;
    if (url.scheme == "file") {
        resolved = url.path;
    } else if (url.scheme == "bundle") {
        auto *maybeRepo = getenv("CK_BUILDDIR");

        if (not maybeRepo)
            maybeRepo = getenv("SKIFT_BUNDLES");

        if (not maybeRepo)
            return Error::notFound("SKIFT_BUNDLES not set");

        auto path = url.path;
        path.rooted = false;

        resolved = Url::Path::parse(maybeRepo)
                       .join(url.host)
                       .join("__res__")
                       .join(path);
    } else {
        return Error::notFound("unknown url scheme");
    }

    return Ok(resolved);
}

Res<Strong<Sys::Fd>> openFile(Url::Url const &url) {
    String str = try$(resolve(url)).str();

    isize raw = ::open(str.buf(), O_RDONLY);
    if (raw < 0)
        return Posix::fromLastErrno();
    auto fd = makeStrong<PosixFd>(raw);
    if (try$(fd->stat()).type == Stat::DIR)
        return Error::isADirectory();
    return Ok(fd);
}

Res<Strong<Sys::Fd>> createFile(Url::Url const &url) {
    String str = try$(resolve(url)).str();

    auto raw = ::open(str.buf(), O_RDWR | O_CREAT, 0644);
    if (raw < 0)
        return Posix::fromLastErrno();
    return Ok(makeStrong<PosixFd>(raw));
}

Res<Strong<Sys::Fd>> openOrCreateFile(Url::Url const &url) {
    String str = try$(resolve(url)).str();

    auto raw = ::open(str.buf(), O_RDWR | O_CREAT, 0644);
    if (raw < 0)
        return Posix::fromLastErrno();
    auto fd = makeStrong<PosixFd>(raw);
    if (try$(fd->stat()).type == Stat::DIR)
        return Error::isADirectory();
    return Ok(fd);
}

Res<Pair<Strong<Sys::Fd>>> createPipe() {
    int fds[2];

    if (::pipe(fds) < 0)
        return Posix::fromLastErrno();

    return Ok(Pair<Strong<Sys::Fd>>{
        makeStrong<PosixFd>(fds[0]),
        makeStrong<PosixFd>(fds[1]),
    });
}

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<PosixFd>(0));
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<PosixFd>(1));
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<PosixFd>(2));
}

Res<Vec<Sys::DirEntry>> readDir(Url::Url const &url) {
    String str = try$(resolve(url)).str();

    DIR *dir = ::opendir(str.buf());
    if (not dir)
        return Posix::fromLastErrno();

    Vec<Sys::DirEntry> entries;
    struct dirent *entry;
    errno = 0;
    while ((entry = ::readdir(dir))) {
        try$(Posix::consumeErrno());

        if (strcmp(entry->d_name, ".") == 0 or
            strcmp(entry->d_name, "..") == 0 or
            (strLen(entry->d_name) >= 1 and entry->d_name[0] == '.')) {
            continue;
        }

        entries.pushBack(Sys::DirEntry{entry->d_name, entry->d_type == DT_DIR});
    }

    if (::closedir(dir) < 0)
        return Posix::fromLastErrno();

    return Ok(entries);
}

Res<Stat> stat(Url::Url const &url) {
    String str = try$(resolve(url)).str();
    struct stat buf;
    if (::stat(str.buf(), &buf) < 0)
        return Posix::fromLastErrno();
    return Ok(fromStat(buf));
}

/* --- Sockets -------------------------------------------------------------- */

Res<Strong<Sys::Fd>> connectTcp(SocketAddr addr) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    struct sockaddr_in addr_ = toSockAddr(addr);
    if (::connect(fd, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
        return Posix::fromLastErrno();

    return Ok(makeStrong<PosixFd>(fd));
}

Res<Strong<Sys::Fd>> listenTcp(SocketAddr addr) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    int opt = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        return Posix::fromLastErrno();

    struct sockaddr_in addr_ = toSockAddr(addr);

    if (::bind(fd, (struct sockaddr *)&addr_, sizeof(addr_)) < 0)
        return Posix::fromLastErrno();

    if (::listen(fd, 128) < 0)
        return Posix::fromLastErrno();

    return Ok(makeStrong<PosixFd>(fd));
}

/* --- Time ----------------------------------------------------------------- */

TimeSpan fromTimeSpec(struct timespec const &ts) {
    auto usecs = (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;
    return TimeSpan::fromUSecs(usecs);
}

TimeStamp now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return TimeStamp::epoch() + fromTimeSpec(ts);
}

TimeSpan uptime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return fromTimeSpec(ts);
}

/* --- Memory Managment ----------------------------------------------------- */

isize mmapOptionsToProt(Sys::MmapOptions const &options) {
    isize prot = 0;

    if (options.flags & Sys::MmapFlags::READ)
        prot |= PROT_READ;

    if (options.flags & Sys::MmapFlags::WRITE)
        prot |= PROT_WRITE;

    if (options.flags & Sys::MmapFlags::EXEC)
        prot |= PROT_EXEC;

    return prot;
}

Res<Sys::MmapResult> memMap(Karm::Sys::MmapOptions const &options) {
    void *addr = mmap((void *)options.vaddr, options.size, mmapOptionsToProt(options), MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (addr == MAP_FAILED)
        return Posix::fromLastErrno();

    if (options.flags & Sys::MmapFlags::PREFETCH) {
        if (madvise(addr, options.size, MADV_WILLNEED) < 0)
            return Posix::fromLastErrno();
    }

    return Ok(Sys::MmapResult{0, (usize)addr, (usize)options.size});
}

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> maybeFd) {
    Strong<PosixFd> fd = try$(maybeFd.cast<PosixFd>());
    usize size = options.size;

    if (size == 0)
        size = try$(Io::size(*fd));

    void *addr = mmap((void *)options.vaddr, size, mmapOptionsToProt(options), MAP_SHARED, fd->_raw, options.offset);

    if (addr == MAP_FAILED)
        return Posix::fromLastErrno();

    return Ok(Sys::MmapResult{0, (usize)addr, (usize)size});
}

Res<> memUnmap(void const *buf, usize len) {
    if (::munmap((void *)buf, len) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<> memFlush(void *flush, usize len) {
    if (::msync(flush, len, MS_INVALIDATE) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<> populate(Sys::SysInfo &infos) {
    struct utsname uts;
    if (uname(&uts) < 0)
        return Posix::fromLastErrno();

    infos.sysName = "Posix";
    infos.sysVersion = try$(Fmt::format("{}", _POSIX_VERSION));

    infos.kernelName = uts.sysname;
    infos.kernelVersion = uts.release;

    infos.hostname = uts.nodename;

    return Ok();
}

Res<> populate(Sys::MemInfo &) {
    return Error::notImplemented();
}

Res<> populate(Vec<Sys::CpuInfo> &) {
    return Error::notImplemented();
}

Res<> populate(Sys::UserInfo &infos) {
    infos.name = getenv("USER");
    infos.home.scheme = "file";
    infos.home.path = Url::Path::parse(getenv("HOME"));

    infos.shell.scheme = "file";
    infos.shell.path = Url::Path::parse(getenv("SHELL"));

    return Ok();
}

Res<> populate(Vec<Sys::UserInfo> &infos) {
    Sys::UserInfo info;
    try$(populate(info));
    infos.pushBack(info);
    return Ok();
}

/* --- Process Managment ---------------------------------------------------- */

Res<> sleep(TimeSpan span) {
    struct timespec ts;
    ts.tv_sec = span.toSecs();
    ts.tv_nsec = span.toUSecs() % 1000000 * 1000;
    if (nanosleep(&ts, nullptr) < 0) {
        return Posix::fromLastErrno();
    }
    return Ok();
}

Res<> exit(i32 res) {
    ::exit(res);
    return Ok();
}

} // namespace Karm::Sys::_Embed
