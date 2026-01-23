module;

#include <hal/mem.h>
#include <vaerk-efi/base.h>

module Karm.Sys;

import Karm.Logger;
import Karm.Core;
import Karm.Ref;

namespace Karm::Sys::_Embed {

static SystemTime fromEfi(Efi::Time time) {
    DateTime dt;
    dt.date.year = time.year;
    dt.date.month = time.month;
    dt.date.day = time.day;
    dt.time.hour = time.hour;
    dt.time.minute = time.minute;
    dt.time.second = time.second;
    return dt.toInstant();
}

struct ConOut : Fd {
    Efi::SimpleTextOutputProtocol* _proto;

    ConOut(Efi::SimpleTextOutputProtocol* proto) : _proto(proto) {}

    Res<usize> read(MutBytes) override {
        return Error::notImplemented();
    }

    Res<usize> write(Bytes bytes) override {
        usize writen{};
        Array<u16, 129> buf{};
        // Some space for the null terminator.
        auto chunkSize = sizeOf(buf) - sizeof(u16);

        while (not isEmpty(bytes)) {
            usize toCopy = alignDown(sizeOf(bytes), sizeof(u16));

            // We need to copy the bytes into to a u16 aligned buffer.
            copy(sub(bytes, 0, toCopy), mutBytes(buf));

            // If bytes.size() is not a multiple of sizeof(u16),
            // then the last byte will be ignored.
            buf[(toCopy / sizeof(u16)) + 1] = 0;

            try$(_proto->outputString(_proto, buf.buf()));
            writen += toCopy;

            bytes = next(bytes, chunkSize);
        }

        return Ok(writen);
    }

    Res<usize> seek(Io::Seek) override {
        return Error::notImplemented();
    }

    Res<> flush() override {
        return Ok();
    }

    Res<Rc<Fd>> dup() override {
        return Error::notImplemented();
    }

    Res<_Accepted> accept() override {
        return Error::notImplemented();
    }

    Res<Stat> stat() override {
        return Ok<Stat>();
    }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        return Error::notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        return Error::notImplemented();
    }
};

struct FileProto : Fd {
    Efi::FileProtocol* _proto = nullptr;

    FileProto(Efi::FileProtocol* proto) : _proto(proto) {}

    ~FileProto() {
        if (_proto) {
            _proto->close(_proto).unwrap("close() failed");
        }
    }

    FileProto(FileProto&& other) {
        std::swap(_proto, other._proto);
    }

    FileProto& operator=(FileProto&& other) {
        std::swap(_proto, other._proto);
        return *this;
    }

    Res<usize> read(MutBytes bytes) override {
        usize size = sizeOf(bytes);
        try$(_proto->read(_proto, &size, bytes.buf()));
        return Ok(size);
    }

    Res<usize> write(Bytes bytes) override {
        usize size = sizeOf(bytes);
        try$(_proto->write(_proto, &size, bytes.buf()));
        return Ok(size);
    }

    Res<usize> seek(Io::Seek seek) override {
        u64 current = 0;
        try$(_proto->getPosition(_proto, &current));

        usize bufSize = 0;
        // NOTE: This is expectected to fail
        (void)_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, nullptr);

        Buf<u8> buf;
        buf.resize(bufSize, 0);

        Efi::FileInfo* info = (Efi::FileInfo*)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, info));

        usize pos = try$(seek.apply(current, info->fileSize));

        if (pos == current) {
            return Ok(current);
        }

        try$(_proto->setPosition(_proto, pos));

        return Ok(pos);
    }

    Res<> flush() override {
        try$(_proto->flush(_proto));
        return Ok();
    }

    Res<Rc<Fd>> dup() override {
        return Error::notImplemented();
    }

    Res<_Accepted> accept() override {
        return Error::notImplemented();
    }

    Res<Stat> stat() override {
        Efi::FileInfo* info = nullptr;
        usize bufSize = 0;
        // NOTE: This is expectected to fail
        (void)_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, nullptr);

        Buf<u8> buf;
        buf.resize(bufSize, 0);

        info = (Efi::FileInfo*)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, info));

        return Ok<Stat>(Stat{
            .type = info->attribute & EFI_FILE_DIRECTORY ? Type::DIR : Type::FILE,
            .size = info->fileSize,
            .accessTime = fromEfi(info->lastAccessTime),
            .modifyTime = fromEfi(info->modificationTime),
            .changeTime = fromEfi(info->createTime),
        });
    }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        return Error::notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        return Error::notImplemented();
    }
};

Res<Rc<Sys::Fd>> deserializeFd(Serde::Deserializer&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> createIn() {
    return Ok(makeRc<NullFd>());
}

Res<Rc<Fd>> createOut() {
    return Ok(makeRc<ConOut>(Efi::st()->conOut));
}

Res<Rc<Fd>> createErr() {
    return Ok(makeRc<ConOut>(Efi::st()->stdErr));
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Fd>> connectTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenUdp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenIpc(Ref::Url) {
    return Error::notImplemented();
}

// MARK: Files -----------------------------------------------------------------

static Res<Ref::Path> resolve(Ref::Url url) {
    if (url.scheme == "file") {
        return Ok(url.path);
    } else if (url.scheme == "bundle") {
        url.path.rooted = false;
        return Ok("/bundles"_path / url.host.str() / url.path);
    } else {
        logError("unsupported scheme: {}", url.scheme);
        return Error::notImplemented();
    }
}

Res<Rc<Fd>> openFile(Ref::Url const& url) {
    static Efi::SimpleFileSystemProtocol* fileSystem = nullptr;
    if (not fileSystem) {
        fileSystem = try$(Efi::openProtocol<Efi::SimpleFileSystemProtocol>(Efi::li()->deviceHandle));
    }

    static Efi::FileProtocol* rootDir = nullptr;
    if (not rootDir) {
        try$(fileSystem->openVolume(fileSystem, &rootDir));
    }

    Efi::FileProtocol* file = nullptr;
    auto resolved = try$(resolve(url)).str();
    logDebug("resolved: {}", resolved);

    _StringBuilder<Utf16> b;
    b.ensure(resolved.len());

    // NOTE: Convert '/' to '\' as EFI uses '\' as path separator
    for (Rune u : iterRunes(resolved)) {
        if (u == '/') {
            u = '\\';
        }
        b.append(u);
    }
    auto utf16str = b.take();

    try$(rootDir->open(rootDir, &file, utf16str.buf(), EFI_FILE_MODE_READ, 0));
    return Ok(makeRc<FileProto>(file));
}

Res<Vec<DirEntry>> readDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<> createDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Vec<DirEntry>> readDirOrCreate(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> createFile(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> openOrCreateFile(Ref::Url const&) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(MmapProps const& options) {
    usize vaddr = 0;

    try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, Hal::pageAlignUp(options.size) / Hal::PAGE_SIZE, &vaddr));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(MmapResult{vaddr, vaddr, options.size});
}

Res<MmapResult> memMap(MmapProps const&, Rc<Fd> fd) {
    usize vaddr = 0;
    usize fileSize = try$(Io::size(*fd));

    try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, Hal::pageAlignUp(fileSize) / Hal::PAGE_SIZE, &vaddr));

    MutBytes bytes = {(u8*)vaddr, fileSize};
    Io::BufWriter writer{bytes};
    try$(Io::copy(*fd, writer));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(MmapResult{vaddr, vaddr, Hal::pageAlignUp(fileSize)});
}

Res<> memUnmap(void const* buf, usize size) {
    try$(Efi::bs()->freePages((u64)buf, size / Hal::PAGE_SIZE));
    return Ok();
}

Res<> memFlush(void*, usize) {
    return Ok();
}

Res<Stat> stat(Ref::Url const& url) {
    auto file = try$(openFile(url));
    return file->stat();
}

usize pageSize() {
    return Hal::PAGE_SIZE;
}

// MARK: Time ------------------------------------------------------------------

SystemTime now() {
    Efi::Time t;

    Efi::rt()->getTime(&t, nullptr).unwrap();

    DateTime dt{
        .date = {
            .day = t.day,
            .month = t.month,
            .year = t.year,
        },
        .time = {
            .second = t.second,
            .minute = t.minute,
            .hour = t.hour,
        },
    };

    return dt.toInstant() + Duration::fromUSecs(t.nanosecond / 1000);
}

Instant instant() {
    // HACK: It's supposed to be a monotonic clock, but we don't have one.
    return Instant{now()._value};
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(SysInfo&) {
    return Error::notImplemented();
}

Res<> populate(MemInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<CpuInfo>&) {
    return Error::notImplemented();
}

Res<> populate(UserInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<UserInfo>&) {
    return Error::notImplemented();
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Sys::Intent) {
    return Error::notImplemented();
}

Async::Task<> launchAsync(Sys::Intent) {
    co_return Error::notImplemented();
}

// MARK: Process ---------------------------------------------------------------

Res<Rc<Pid>> run(Command const&) {
    return Error::notImplemented();
}

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(Duration) {
    return Error::notImplemented();
}

Res<> sleepUntil(Instant) {
    return Error::notImplemented();
}

Duration uptime() {
    notImplemented();
}

Res<> exit(i32) {
    Efi::st()
        ->runtime
        ->resetSystem(
            Efi::ResetType::RESET_SHUTDOWN,
            Efi::ERR_ABORTED,
            0,
            nullptr
        )
        .unwrap();

    while (1)
        ;
}

Res<Ref::Url> pwd() {
    return Ok("file:/"_url);
}

// MARK: Sandboxing ------------------------------------------------------------

Res<> hardenSandbox() {
    logInfo("could not harden sandbox");
    return Ok();
}

// MARK: Addr ------------------------------------------------------------------

Async::Task<Vec<Ip>> ipLookupAsync(Str) {
    co_return Error::notImplemented();
}

// MARK: Bundle ----------------------------------------------------------------

Res<Vec<String>> installedBundles() {
    return Error::notImplemented("not implemented");
}

Res<String> currentBundle() {
    return Error::notImplemented("not implemented");
}

} // namespace Karm::Sys::_Embed
