#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

//
#include <karm-sys/_embed.h>
#include <karm-sys/addr.h>
#include <karm-sys/chan.h>
#include <karm-sys/launch.h>
#include <karm-sys/proc.h>

#include "fd.h"
#include "utils.h"

import Karm.Core;

namespace Karm::Sys::_Embed {

Res<Ref::Path> resolve(Ref::Url const& url) {
    Ref::Path resolved;
    if (url.scheme == "file") {
        resolved = url.path;
    } else if (url.scheme == "fd") {
        if (url.path == "stdin"_path) {
            resolved = "/dev/stdin"_path;
        } else if (url.path == "stdout"_path) {
            resolved = "/dev/stdout"_path;
        } else if (url.path == "stderr"_path) {
            resolved = "/dev/stderr"_path;
        } else {
            return Error::notFound("unknown fd");
        }
    } else if (url.scheme == "ipc") {
        auto const* runtimeDir = getenv("XDG_RUNTIME_DIR");
        if (not runtimeDir) {
            runtimeDir = "/tmp/";
            Sys::errln("XDG_RUNTIME_DIR not set, falling back on {}", runtimeDir);
        }

        auto path = url.path;
        path.rooted = false;

        resolved = Ref::Path::parse(runtimeDir).join(path);
    } else if (url.scheme == "bundle") {
        auto [repo, format] = try$(Posix::repoRoot());

        auto path = url.path;
        path.rooted = false;

        if (format == Posix::RepoType::CUTEKIT) {
            resolved = Ref::Path::parse(repo)
                           .join(url.host.str())
                           .join("__res__")
                           .join(path);
        } else if (format == Posix::RepoType::PREFIX) {
            resolved = Ref::Path::parse(repo)
                           .join("share")
                           .join(url.host.str())
                           .join(path);
        } else {
            return Error::notFound("unknown repo type");
        }
    } else if (url.scheme == "location") {
        auto* maybeHome = getenv("HOME");
        if (not maybeHome)
            return Error::notFound("HOME not set");

        auto path = url.path;
        path.rooted = false;

        if (url.host == "home")
            resolved = Ref::Path::parse(maybeHome).join(path);
        else
            resolved = Ref::Path::parse(maybeHome).join(Io::toPascalCase(url.host.str()).unwrap()).join(path);
    } else {
        return Error::notFound("unknown url scheme");
    }

    return Ok(resolved);
}

// MARK: Fd --------------------------------------------------------------------

Res<Rc<Fd>> unpackFd(MessageReader& s) {
    auto handle = s.take();
    if (handle == INVALID)
        return Error::invalidHandle();
    return Ok(makeRc<Posix::Fd>(handle.value()));
}

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Fd>> openFile(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    isize raw = ::open(str.buf(), O_RDONLY);
    if (raw < 0)
        return Posix::fromLastErrno();
    auto fd = makeRc<Posix::Fd>(raw);
    if (try$(fd->stat()).type == Type::DIR)
        return Error::isADirectory();
    return Ok(fd);
}

Res<Rc<Fd>> createFile(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    auto raw = ::open(str.buf(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (raw < 0)
        return Posix::fromLastErrno();
    return Ok(makeRc<Posix::Fd>(raw));
}

Res<Rc<Fd>> openOrCreateFile(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    auto raw = ::open(str.buf(), O_RDWR | O_CREAT, 0644);
    if (raw < 0)
        return Posix::fromLastErrno();
    auto fd = makeRc<Posix::Fd>(raw);
    if (try$(fd->stat()).type == Type::DIR)
        return Error::isADirectory();
    return Ok(fd);
}

Res<Pair<Rc<Fd>>> createPipe() {
    int fds[2];

    if (::pipe(fds) < 0)
        return Posix::fromLastErrno();

    return Ok(Pair<Rc<Fd>>{
        makeRc<Posix::Fd>(fds[0]),
        makeRc<Posix::Fd>(fds[1]),
    });
}

Res<Rc<Fd>> createIn() {
    auto fd = makeRc<Posix::Fd>(0);
    fd->_leak = true; // Don't close stdin when we close the fd
    return Ok(fd);
}

Res<Rc<Fd>> createOut() {
    auto fd = makeRc<Posix::Fd>(1);
    fd->_leak = true; // Don't close stdout when we close the fd
    return Ok(fd);
}

Res<Rc<Fd>> createErr() {
    auto fd = makeRc<Posix::Fd>(2);
    fd->_leak = true; // Don't close stderr when we close the fd
    return Ok(fd);
}

Res<Vec<DirEntry>> readDir(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    DIR* dir = ::opendir(str.buf());
    if (not dir)
        return Posix::fromLastErrno();

    Vec<DirEntry> entries;
    dirent* entry;
    errno = 0;
    while ((entry = ::readdir(dir))) {
        try$(Posix::consumeErrno());

        if (std::strcmp(entry->d_name, ".") == 0 or
            std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        entries.pushBack(DirEntry{
            Str::fromNullterminated(entry->d_name),
            entry->d_type == DT_DIR ? Sys::Type::DIR : Sys::Type::FILE,
        });
    }

    if (::closedir(dir) < 0)
        return Posix::fromLastErrno();

    return Ok(entries);
}

Res<> createDir(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    if (::mkdir(str.buf(), 0755) < 0) {
        if (errno == EEXIST) {
            return Error::alreadyExists("directory already exists");
        }
        return Posix::fromLastErrno();
    }

    return Ok();
}

Res<Vec<Sys::DirEntry>> readDirOrCreate(Ref::Url const& url) {
    String str = try$(resolve(url)).str();

    DIR* dir = ::opendir(str.buf());
    Defer _{[&]() {
        if (dir)
            ::closedir(dir);
    }};

    if (dir) {
        Vec<DirEntry> entries;
        dirent* entry;
        errno = 0;
        while ((entry = ::readdir(dir))) {
            try$(Posix::consumeErrno());

            if (std::strcmp(entry->d_name, ".") == 0 or
                std::strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            entries.pushBack(DirEntry{
                Str::fromNullterminated(entry->d_name),
                entry->d_type == DT_DIR ? Sys::Type::DIR : Sys::Type::FILE,
            });
        }

        return Ok(entries);
    } else {
        if (errno != ENOENT)
            return Posix::fromLastErrno();
        try$(createDir(url));
        return Ok(Vec<DirEntry>{});
    }
}

Res<Stat> stat(Ref::Url const& url) {
    String str = try$(resolve(url)).str();
    struct stat buf;
    if (::stat(str.buf(), &buf) < 0)
        return Posix::fromLastErrno();
    return Ok(Posix::fromStat(buf));
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent intent) {
    if (intent.objects.len() != 1)
        return Error::invalidInput("expected exactly one object");

    auto [url, _] = intent.objects[0];

    String str = try$(resolve(url)).str();

    int pid = fork();
    if (pid < 0)
        return Posix::fromLastErrno();

    if (pid == 0) {
#ifdef __ck_sys_darwin__
        if (intent.action == Ref::Uti::PUBLIC_MODIFY) {
            execlp("open", "open", "-t", str.buf(), nullptr);
        } else {
            execlp("open", "open", str.buf(), nullptr);
        }
#else
        execlp("xdg-open", "xdg-open", str.buf(), nullptr);
#endif
        _exit(1);
    }

    auto status = 0;
    if (waitpid(pid, &status, 0) < 0)
        return Posix::fromLastErrno();

    if (WIFEXITED(status) and WEXITSTATUS(status) == 0)
        return Ok();

    return Posix::fromStatus(status);
}

Async::Task<> launchAsync(Intent intent) {
    co_return _Embed::launch(intent);
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Fd>> listenUdp(SocketAddr addr) {
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    struct sockaddr_in addr_ = Posix::toSockAddr(addr);

    if (::bind(fd, (struct sockaddr*)&addr_, sizeof(addr_)) < 0)
        return Posix::fromLastErrno();

    return Ok(makeRc<Posix::Fd>(fd));
}

Res<Rc<Fd>> connectTcp(SocketAddr addr) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    struct sockaddr_in addr_ = Posix::toSockAddr(addr);
    if (::connect(fd, (struct sockaddr*)&addr_, sizeof(addr_)) < 0)
        return Posix::fromLastErrno();

    return Ok(makeRc<Posix::Fd>(fd));
}

Res<Rc<Fd>> listenTcp(SocketAddr addr) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    int opt = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        return Posix::fromLastErrno();

    struct sockaddr_in addr_ = Posix::toSockAddr(addr);

    if (::bind(fd, (struct sockaddr*)&addr_, sizeof(addr_)) < 0)
        return Posix::fromLastErrno();

    if (::listen(fd, 128) < 0)
        return Posix::fromLastErrno();

    return Ok(makeRc<Posix::Fd>(fd));
}

Res<Rc<Fd>> listenIpc(Ref::Url url) {
    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
        return Posix::fromLastErrno();

    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    String path = try$(resolve(url)).str();
    auto sunPath = MutSlice(addr.sun_path, sizeof(addr.sun_path) - 1);
    copy(sub(path), sunPath);

    if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        return Posix::fromLastErrno();

    if (::listen(fd, 128) < 0)
        return Posix::fromLastErrno();

    return Ok(makeRc<Posix::Fd>(fd));
}

// MARK: Time ------------------------------------------------------------------

Duration fromTimeSpec(struct timespec const& ts) {
    auto usecs = (u64)ts.tv_sec * 1000000 + (u64)ts.tv_nsec / 1000;
    return Duration::fromUSecs(usecs);
}

SystemTime now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return SystemTime::epoch() + fromTimeSpec(ts);
}

Instant instant() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return Instant::epoch() + fromTimeSpec(ts);
}

Duration uptime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return fromTimeSpec(ts);
}

// MARK: Memory Managment ------------------------------------------------------

isize MmapPropsToProt(MmapProps const& options) {
    isize prot = 0;

    if (options.options & MmapOption::READ)
        prot |= PROT_READ;

    if (options.options & MmapOption::WRITE)
        prot |= PROT_WRITE;

    if (options.options & MmapOption::EXEC)
        prot |= PROT_EXEC;

    return prot;
}

Res<MmapResult> memMap(MmapProps const& options) {
    void* addr = mmap((void*)options.vaddr, options.size, MmapPropsToProt(options), MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (addr == MAP_FAILED)
        return Posix::fromLastErrno();

    if (options.options & MmapOption::PREFETCH) {
        if (madvise(addr, options.size, MADV_WILLNEED) < 0)
            return Posix::fromLastErrno();
    }

    return Ok(MmapResult{0, (usize)addr, (usize)options.size});
}

Res<MmapResult> memMap(MmapProps const& options, Rc<Fd> maybeFd) {
    Rc<Posix::Fd> fd = try$(maybeFd.cast<Posix::Fd>());
    usize size = options.size;

    if (size == 0)
        size = try$(Io::size(*fd));

    void* addr = mmap((void*)options.vaddr, size, MmapPropsToProt(options), MAP_SHARED, fd->_raw, options.offset);

    if (addr == MAP_FAILED)
        return Posix::fromLastErrno();

    return Ok(MmapResult{0, (usize)addr, (usize)size});
}

Res<> memUnmap(void const* buf, usize len) {
    if (::munmap((void*)buf, len) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<> memFlush(void* flush, usize len) {
    if (::msync(flush, len, MS_INVALIDATE) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<> populate(SysInfo& infos) {
    struct utsname uts;
    if (uname(&uts) < 0)
        return Posix::fromLastErrno();

    infos.sysName = "Posix"s;
    infos.sysVersion = Io::format("{}", _POSIX_VERSION);

    infos.kernelName = Str::fromNullterminated(uts.sysname, sizeof(uts.sysname));
    infos.kernelVersion = Str::fromNullterminated(uts.release, sizeof(uts.release));

    infos.hostname = Str::fromNullterminated(uts.nodename, sizeof(uts.nodename));

    return Ok();
}

Res<> populate(MemInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<CpuInfo>&) {
    return Error::notImplemented();
}

Res<> populate(UserInfo& infos) {
    infos.name = Str::fromNullterminated(getenv("USER"));
    infos.home.scheme = "file"_sym;
    infos.home.path = Ref::Path::parse(getenv("HOME"));

    infos.shell.scheme = "file"_sym;
    infos.shell.path = Ref::Path::parse(getenv("SHELL"));

    return Ok();
}

Res<> populate(Vec<UserInfo>& infos) {
    UserInfo info;
    try$(populate(info));
    infos.pushBack(info);
    return Ok();
}

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(Duration span) {
    struct timespec ts;
    ts.tv_sec = span.toSecs();
    ts.tv_nsec = (span.toUSecs() % 1000000) * 1000;
    if (nanosleep(&ts, nullptr) < 0)
        return Posix::fromLastErrno();
    return Ok();
}

Res<> exit(i32 res) {
    ::exit(res);
    return Error::other("reached the afterlife");
}

Res<Ref::Url> pwd() {
    auto buf = Buf<char>::init(256);
    while (true) {
        if (::getcwd(buf.buf(), buf.len()) != NULL)
            break;

        if (errno != ERANGE)
            return Posix::fromLastErrno();

        buf.resize(buf.len() * 2);
    }

    return Ok(Ref::parseUrlOrPath(Str::fromNullterminated(buf.buf()), "file:"_url));
}

// MARK: Addr ------------------------------------------------------------------

Async::Task<Vec<Ip>> ipLookupAsync(Str host) {
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    Vec<Ip> ips;
    if (host == "localhost") {
        ips.pushBack(Ip4::localhost());
        ips.pushBack(Ip6::localhost());
        co_return Ok(ips);
    }

    struct addrinfo* res;
    if (getaddrinfo(host.buf(), nullptr, &hints, &res) < 0)
        co_return Posix::fromLastErrno();

    for (auto* p = res; p; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
            ips.pushBack(Ip4::fromRaw(bswap(addr->sin_addr.s_addr)));
        } else if (p->ai_family == AF_INET6) {
            struct sockaddr_in6* addr = (struct sockaddr_in6*)p->ai_addr;
            u128 raw = 0;
            u16 const* buf = (u16 const*)addr->sin6_addr.s6_addr;
            for (usize i = 0; i < 8; i++)
                raw |= (u128)buf[i] << (i * 16);
            ips.pushBack(Ip6::fromRaw(bswap(raw)));
        }
    }

    freeaddrinfo(res);
    co_return Ok(ips);
}

} // namespace Karm::Sys::_Embed
