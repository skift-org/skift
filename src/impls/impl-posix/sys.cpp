#include <karm-io/funcs.h>
#include <karm-logger/logger.h>

#include <karm-sys/_embed.h>

/* Posix Stuff*/
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

#include "errno.h"

namespace Karm::Sys::_Embed {

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
};

Res<Url::Path> resolve(Url::Url url) {
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
                       .join("res")
                       .join(path);
    } else {
        return Error::notFound("unknown url scheme");
    }

    return Ok(resolved);
}

Res<Strong<Sys::Fd>> openFile(Url::Url url) {
    String str = try$(try$(resolve(url)).str());
    isize fd = ::open(str.buf(), O_RDONLY);

    if (fd < 0)
        return Posix::fromLastErrno();

    return Ok(makeStrong<PosixFd>(fd));
}

Res<Vec<Sys::DirEntry>> readDir(Url::Url url) {
    String str = try$(try$(resolve(url)).str());

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

Res<Strong<Sys::Fd>> createFile(Url::Url url) {
    String str = try$(try$(resolve(url)).str());

    auto fd = ::open(str.buf(), O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return Posix::fromLastErrno();
    }

    return Ok(makeStrong<PosixFd>(fd));
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
