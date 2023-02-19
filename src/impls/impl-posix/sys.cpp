#include <dirent.h>
#include <embed-sys/sys.h>
#include <fcntl.h>
#include <karm-io/funcs.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "errno.h"

namespace Embed {

struct PosixFd : public Sys::Fd {
    int _raw;

    PosixFd(int raw) : _raw(raw) {}

    Res<size_t> read(MutBytes bytes) override {
        ssize_t result = ::read(_raw, bytes.buf(), sizeOf(bytes));

        if (result < 0) {
            return Posix::fromLastErrno();
        }

        return Ok(static_cast<size_t>(result));
    }

    Res<size_t> write(Bytes bytes) override {
        ssize_t result = ::write(_raw, bytes.buf(), sizeOf(bytes));

        if (result < 0) {
            return Posix::fromLastErrno();
        }

        return Ok(static_cast<size_t>(result));
    }

    Res<size_t> seek(Io::Seek seek) override {
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

        if (offset < 0) {
            return Posix::fromLastErrno();
        }

        return Ok(static_cast<size_t>(offset));
    }

    Res<size_t> flush() override {
        // NOTE: No-op
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        int duped = ::dup(_raw);

        if (duped < 0) {
            return Posix::fromLastErrno();
        }

        return Ok(makeStrong<PosixFd>(duped));
    }
};

Res<Strong<Sys::Fd>> openFile(Sys::Path path) {
    String str = path.str();
    int fd = ::open(str.buf(), O_RDONLY);

    if (fd < 0) {
        return Posix::fromLastErrno();
    }

    return Ok(makeStrong<PosixFd>(fd));
}

Res<Vec<Sys::DirEntry>> readDir(Sys::Path path) {
    String str = path.str();
    DIR *dir = ::opendir(str.buf());
    if (not dir) {
        return Posix::fromLastErrno();
    }
    Vec<Sys::DirEntry> entries;
    struct dirent *entry;
    while ((entry = ::readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        entries.pushBack(Sys::DirEntry{entry->d_name, entry->d_type == DT_DIR});
    }
    ::closedir(dir);
    return Ok(entries);
}

Res<Strong<Sys::Fd>> createFile(Sys::Path path) {
    String str = path.str();

    int fd = ::open(str.buf(), O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return Posix::fromLastErrno();
    }

    return Ok(makeStrong<PosixFd>(fd));
}

Res<Pair<Strong<Sys::Fd>>> createPipe() {
    int fds[2];

    if (::pipe(fds) < 0) {
        return Posix::fromLastErrno();
    }

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

int mmapOptionsToProt(Sys::MmapOptions const &options) {
    int prot = 0;

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

    if (addr == MAP_FAILED) {
        return Posix::fromLastErrno();
    }

    if (options.flags & Sys::MmapFlags::PREFETCH) {
        madvise(addr, options.size, MADV_WILLNEED);
    }

    return Ok(Sys::MmapResult{0, (size_t)addr, (size_t)options.size});
}

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> maybeFd) {
    Strong<PosixFd> fd = try$(maybeFd.as<PosixFd>());
    size_t size = options.size;

    if (size == 0) {
        size = try$(Io::size(*fd));
    }

    void *addr = mmap((void *)options.vaddr, size, mmapOptionsToProt(options), MAP_SHARED, fd->_raw, options.offset);

    if (addr == MAP_FAILED) {
        return Posix::fromLastErrno();
    }

    return Ok(Sys::MmapResult{0, (size_t)addr, (size_t)size});
}

Res<> memUnmap(void const *buf, size_t len) {
    if (::munmap((void *)buf, len) < 0) {
        return Posix::fromLastErrno();
    }

    return Ok();
}

Res<> memFlush(void *flush, size_t len) {
    if (::msync(flush, len, MS_INVALIDATE) < 0) {
        return Posix::fromLastErrno();
    }

    return Ok();
}

Res<> populate(Sys::SysInfo &infos) {
    struct utsname uts;
    if (uname(&uts) < 0) {
        return Posix::fromLastErrno();
    }

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
    infos.home = getenv("HOME");
    infos.shell = getenv("SHELL");

    return Ok();
}

Res<> populate(Vec<Sys::UserInfo> &infos) {
    Sys::UserInfo info;
    try$(populate(info));
    infos.pushBack(info);
    return Ok();
}

} // namespace Embed
