#include <efi/base.h>
#include <hal/mem.h>
#include <karm-base/align.h>
#include <karm-io/funcs.h>
#include <karm-io/impls.h>
#include <karm-json/parse.h>
#include <karm-logger/logger.h>
#include <karm-sys/_embed.h>
#include <karm-sys/file.h>
#include <karm-sys/launch.h>

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

struct ConOut : public Fd {
    Efi::SimpleTextOutputProtocol* _proto;

    ConOut(Efi::SimpleTextOutputProtocol* proto) : _proto(proto) {}

    Handle handle() const override {
        return Handle{(usize)_proto};
    }

    Res<usize> read(MutBytes) override {
        notImplemented();
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
        notImplemented();
    }

    Res<> flush() override {
        return Ok();
    }

    Res<Rc<Fd>> dup() override {
        notImplemented();
    }

    Res<_Accepted> accept() override {
        notImplemented();
    }

    Res<Stat> stat() override {
        return Ok<Stat>();
    }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        notImplemented();
    }

    Res<> pack(Io::PackEmit&) override {
        notImplemented();
    }
};

struct FileProto : public Fd {
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

    Handle handle() const override {
        return Handle((usize)_proto);
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

        usize pos = seek.apply(current, info->fileSize);

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
        notImplemented();
    }

    Res<_Accepted> accept() override {
        notImplemented();
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
        notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        notImplemented();
    }

    Res<> pack(Io::PackEmit&) override {
        notImplemented();
    }
};

Res<Rc<Fd>> unpackFd(Io::PackScan&) {
    notImplemented();
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
    notImplemented();
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    notImplemented();
}

Res<Rc<Fd>> listenUdp(SocketAddr) {
    notImplemented();
}

Res<Rc<Fd>> listenIpc(Mime::Url) {
    notImplemented();
}

// MARK: Files -----------------------------------------------------------------

static Opt<Json::Value> _index = NONE;

static Res<Mime::Path> resolve(Mime::Url url) {
    if (url.scheme == "file") {
        return Ok(url.path);
    }

    if (url.scheme == "bundle") {
        logInfo("resolving bundle url: {}", url);
        if (not _index) {
            logInfo("no index, loading");

            auto indexFile = try$(File::open("file:/bundles/_index.json"_url));
            auto indexStr = try$(Io::readAllUtf8(indexFile));
            auto indexJson = try$(Json::parse(indexStr));

            _index = indexJson.get("objects");

            logInfo("index loaded, might not be valid will check later");
        }

        if (not _index->isObject()) {
            logError("invalid index");
            return Error::invalidData();
        }

        auto path = url.path;
        path.rooted = false;

        auto key = url.str();
        auto object = _index->get(key);

        if (not object.isObject()) {
            logError("invalid object");
            return Error::invalidData("invalid object");
        }

        auto ref = object.get("ref");

        if (not ref.isStr()) {
            logError("invalid ref");
            return Error::invalidData("invalid ref");
        }

        auto refStr = ref.asStr();

        auto refUrl = Mime::Url::parse(refStr);

        Mime::Path resolved = try$(resolve(refUrl));
        logInfo("resolved to: {}", resolved);
        return Ok(resolved);
    } else {
        logError("unsupported scheme: {}", url.scheme);
        return Error::notImplemented();
    }
}

Res<Rc<Fd>> openFile(Mime::Url const& url) {
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

Res<Vec<DirEntry>> readDir(Mime::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> createFile(Mime::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> openOrCreateFile(Mime::Url const&) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(MmapOptions const& options) {
    usize vaddr = 0;

    try$(Efi::bs()->allocatePages(Efi::AllocateType::ANY_PAGES, Efi::MemoryType::LOADER_DATA, Hal::pageAlignUp(options.size) / Hal::PAGE_SIZE, &vaddr));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(MmapResult{vaddr, vaddr, options.size});
}

Res<MmapResult> memMap(MmapOptions const&, Rc<Fd> fd) {
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

Res<Stat> stat(Mime::Url const&) {
    notImplemented();
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

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(Duration) {
    return Error::notImplemented();
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

Res<Mime::Url> pwd() {
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

} // namespace Karm::Sys::_Embed
