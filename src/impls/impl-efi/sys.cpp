#include <efi/base.h>
#include <hal/mem.h>
#include <karm-base/align.h>
#include <karm-io/funcs.h>
#include <karm-io/impls.h>
#include <karm-logger/logger.h>
#include <karm-net/json/json.h>
#include <karm-sys/file.h>

#include <karm-sys/_embed.h>

namespace Karm::Sys::_Embed {

static TimeStamp fromEfi(Efi::Time time) {
    DateTime dt;
    dt.date.year = time.year;
    dt.date.month = time.month;
    dt.date.day = time.day;
    dt.time.hour = time.hour;
    dt.time.minute = time.minute;
    dt.time.second = time.second;
    return dt.toTimeStamp();
}

struct ConOut : public Fd {
    Efi::SimpleTextOutputProtocol *_proto;

    ConOut(Efi::SimpleTextOutputProtocol *proto) : _proto(proto) {}

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

    Res<usize> flush() override {
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
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

    Res<> pack(Io::PackEmit &) override {
        notImplemented();
    }
};

struct FileProto : public Fd {
    Efi::FileProtocol *_proto = nullptr;

    FileProto(Efi::FileProtocol *proto) : _proto(proto) {}

    ~FileProto() {
        if (_proto) {
            _proto->close(_proto).unwrap("close() failled");
        }
    }

    FileProto(FileProto &&other) {
        std::swap(_proto, other._proto);
    }

    FileProto &operator=(FileProto &&other) {
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

        Efi::FileInfo *info = (Efi::FileInfo *)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, info));

        usize pos = seek.apply(current, info->fileSize);

        if (pos == current) {
            return Ok(current);
        }

        try$(_proto->setPosition(_proto, pos));

        return Ok(pos);
    }

    Res<usize> flush() override {
        try$(_proto->flush(_proto));
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        notImplemented();
    }

    Res<_Accepted> accept() override {
        notImplemented();
    }

    Res<Stat> stat() override {
        Efi::FileInfo *info = nullptr;
        usize bufSize = 0;
        // NOTE: This is expectected to fail
        (void)_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, nullptr);

        Buf<u8> buf;
        buf.resize(bufSize, 0);

        info = (Efi::FileInfo *)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::GUID, &bufSize, info));

        return Ok<Stat>(Stat{
            .type = info->attribute & EFI_FILE_DIRECTORY ? Stat::DIR : Stat::FILE,
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

    Res<> pack(Io::PackEmit &) override {
        notImplemented();
    }
};

Res<Strong<Fd>> unpackFd(Io::PackScan &) {
    notImplemented();
}

Res<Strong<Fd>> createIn() {
    return Ok(makeStrong<NullFd>());
}

Res<Strong<Fd>> createOut() {
    return Ok(makeStrong<ConOut>(Efi::st()->conOut));
}

Res<Strong<Fd>> createErr() {
    return Ok(makeStrong<ConOut>(Efi::st()->stdErr));
}

// MARK: Sockets ---------------------------------------------------------------

Res<Strong<Fd>> connectTcp(SocketAddr) {
    notImplemented();
}

Res<Strong<Fd>> listenTcp(SocketAddr) {
    notImplemented();
}

Res<Strong<Fd>> listenUdp(SocketAddr) {
    notImplemented();
}

Res<Strong<Fd>> listenIpc(Mime::Url) {
    notImplemented();
}

// MARK: Files -----------------------------------------------------------------

static Opt<Net::Json::Value> _index = NONE;

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
            auto indexJson = try$(Net::Json::parse(indexStr));

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

Res<Strong<Fd>> openFile(Mime::Url const &url) {
    static Efi::SimpleFileSystemProtocol *fileSystem = nullptr;
    if (not fileSystem) {
        fileSystem = try$(Efi::openProtocol<Efi::SimpleFileSystemProtocol>(Efi::li()->deviceHandle));
    }

    static Efi::FileProtocol *rootDir = nullptr;
    if (not rootDir) {
        try$(fileSystem->openVolume(fileSystem, &rootDir));
    }

    Efi::FileProtocol *file = nullptr;
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
    return Ok(makeStrong<FileProto>(file));
}

Res<Vec<DirEntry>> readDir(Mime::Url const &) {
    return Error::notImplemented();
}

Res<Strong<Fd>> createFile(Mime::Url const &) {
    return Error::notImplemented();
}

Res<Strong<Fd>> openOrCreateFile(Mime::Url const &) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(MmapOptions const &options) {
    usize vaddr = 0;

    try$(Efi::bs()->allocatePages(
        Efi::AllocateType::ANY_PAGES,
        Efi::MemoryType::LOADER_DATA,
        Hal::pageAlignUp(options.size) / Hal::PAGE_SIZE,
        &vaddr
    ));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(MmapResult{vaddr, vaddr, options.size});
}

Res<MmapResult> memMap(MmapOptions const &, Strong<Fd> fd) {
    usize vaddr = 0;
    usize fileSize = try$(Io::size(*fd));

    try$(Efi::bs()->allocatePages(
        Efi::AllocateType::ANY_PAGES,
        Efi::MemoryType::LOADER_DATA,
        Hal::pageAlignUp(fileSize) / Hal::PAGE_SIZE, &vaddr
    ));

    MutBytes bytes = {(Byte *)vaddr, fileSize};
    Io::BufWriter writer{bytes};
    try$(Io::copy(*fd, writer));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(MmapResult{vaddr, vaddr, Hal::pageAlignUp(fileSize)});
}

Res<> memUnmap(void const *buf, usize size) {
    try$(Efi::bs()->freePages((u64)buf, size / Hal::PAGE_SIZE));
    return Ok();
}

Res<> memFlush(void *, usize) {
    return Ok();
}

Res<Stat> stat(Mime::Url const &) {
    notImplemented();
}

// MARK: Time ------------------------------------------------------------------

TimeStamp now() {
    Efi::Time t;
    Efi::rt()->getTime(&t, nullptr).unwrap();

    Date date = {
        t.day,
        t.month,
        t.year,
    };

    Time time = {
        t.hour,
        t.minute,
        t.second,
    };

    return DateTime{date, time}.toTimeStamp() +
           TimeSpan::fromUSecs(t.nanosecond / 1000);
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(SysInfo &) {
    return Error::notImplemented();
}

Res<> populate(MemInfo &) {
    return Error::notImplemented();
}

Res<> populate(Vec<CpuInfo> &) {
    return Error::notImplemented();
}

Res<> populate(UserInfo &) {
    return Error::notImplemented();
}

Res<> populate(Vec<UserInfo> &) {
    return Error::notImplemented();
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Mime::Uti const &, Mime::Url const &) {
    notImplemented();
}

Async::Task<> launchAsync(Mime::Uti const &, Mime::Url const &) {
    notImplemented();
}

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(TimeSpan) {
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

} // namespace Karm::Sys::_Embed
