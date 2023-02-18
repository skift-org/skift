#include <efi/base.h>
#include <embed-sys/sys.h>
#include <hal/mem.h>
#include <karm-base/align.h>
#include <karm-base/macros.h>
#include <karm-io/funcs.h>
#include <karm-io/impls.h>

namespace Embed {

struct ConOut : public Sys::Fd {
    Efi::SimpleTextOutputProtocol *_proto;

    ConOut(Efi::SimpleTextOutputProtocol *proto) : _proto(proto) {}

    Res<size_t> read(MutBytes) override {
        notImplemented();
    }

    Res<size_t> write(Bytes bytes) override {
        size_t writen{};
        Array<uint16_t, 129> buf{};
        // Some space for the null terminator.
        auto chunkSize = sizeOf(buf) - sizeof(uint16_t);

        while (not isEmpty(bytes)) {
            size_t toCopy = alignDown(sizeOf(bytes), sizeof(uint16_t));

            // We need to copy the bytes into to a uint16_t aligned buffer.
            copy(sub(bytes, 0, toCopy), mutBytes(buf));

            // If bytes.size() is not a multiple of sizeof(uint16_t),
            // then the last byte will be ignored.
            buf[(toCopy / sizeof(uint16_t)) + 1] = 0;

            writen += try$(_proto->outputString(_proto, buf.buf()));

            bytes = next(bytes, chunkSize);
        }

        return Ok(writen);
    }

    Res<size_t> seek(Io::Seek) override {
        notImplemented();
    }

    Res<size_t> flush() override {
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        notImplemented();
    }
};

struct FileProto : public Sys::Fd, Meta::Static {
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

    Res<size_t> read(MutBytes bytes) override {
        size_t size = sizeOf(bytes);
        try$(_proto->read(_proto, &size, bytes.buf()));
        return Ok(size);
    }

    Res<size_t> write(Bytes bytes) override {
        size_t size = sizeOf(bytes);
        try$(_proto->write(_proto, &size, bytes.buf()));
        return Ok(size);
    }

    Res<size_t> seek(Io::Seek seek) override {
        uint64_t current = 0;
        try$(_proto->getPosition(_proto, &current));

        size_t bufSize;
        // NOTE: This is expectected to fail
        (void)_proto->getInfo(_proto, &Efi::FileInfo::UUID, &bufSize, nullptr);

        Buf<uint8_t> buf;
        buf.resize(bufSize, 0);

        Efi::FileInfo *info = (Efi::FileInfo *)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::UUID, &bufSize, info));

        size_t pos = seek.apply(current, info->fileSize);

        if (pos == current) {
            return Ok(current);
        }

        try$(_proto->setPosition(_proto, pos));

        return Ok(pos);
    }

    Res<size_t> flush() override {
        try$(_proto->flush(_proto));
        return Ok(0uz);
    }

    Res<Strong<Fd>> dup() override {
        notImplemented();
    }
};

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<ConOut>(Efi::st()->conOut));
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<ConOut>(Efi::st()->stdErr));
}

Res<Strong<Sys::Fd>> openFile(Sys::Path path) {
    static Efi::SimpleFileSystemProtocol *fileSystem = nullptr;
    if (not fileSystem) {
        fileSystem = try$(Efi::openProtocol<Efi::SimpleFileSystemProtocol>(Efi::li()->deviceHandle));
    }

    static Efi::FileProtocol *rootDir = nullptr;
    if (not rootDir) {
        try$(fileSystem->openVolume(fileSystem, &rootDir));
    }

    Efi::FileProtocol *file = nullptr;
    _String<Utf16> pathStr = transcode<Utf16>(path.str());

    // NOTE: Convert '/' to '\' as EFI uses '\' as path separator
    for (auto &u : pathStr) {
        if (u == '/') {
            u = '\\';
        }
    }

    try$(rootDir->open(rootDir, &file, pathStr.buf(), EFI_FILE_MODE_READ, 0));
    return Ok(makeStrong<FileProto>(file));
}

Res<Vec<Sys::DirEntry>> readDir(Sys::Path) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<Strong<Sys::Fd>> createFile(Sys::Path) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<Sys::MmapResult> memMap(Karm::Sys::MmapOptions const &options) {
    size_t vaddr = 0;

    try$(Efi::bs()->allocatePages(
        Efi::AllocateType::ANY_PAGES,
        Efi::MemoryType::LOADER_DATA,
        Hal::pageAlignUp(options.size) / Hal::PAGE_SIZE,
        &vaddr));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(Sys::MmapResult{vaddr, vaddr, options.size});
}

Res<Sys::MmapResult> memMap(Karm::Sys::MmapOptions const &, Strong<Sys::Fd> fd) {
    size_t vaddr = 0;
    size_t fileSize = try$(Io::size(*fd));

    try$(Efi::bs()->allocatePages(
        Efi::AllocateType::ANY_PAGES,
        Efi::MemoryType::LOADER_DATA,
        Hal::pageAlignUp(fileSize) / Hal::PAGE_SIZE, &vaddr));

    MutBytes bytes = {(Byte *)vaddr, fileSize};
    Io::BufWriter writer{bytes};
    try$(Io::copy(*fd, writer));

    // Memory is identity mapped, so we can just return the virtual address as paddr
    return Ok(Sys::MmapResult{vaddr, vaddr, Hal::pageAlignUp(fileSize)});
}

Res<> memUnmap(void const *buf, size_t size) {
    try$(Efi::bs()->freePages((uint64_t)buf, size / Hal::PAGE_SIZE));
    return Ok();
}

Res<> memFlush(void *, size_t) {
    return Ok();
}

Res<> populate(Sys::SysInfo &) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<> populate(Sys::MemInfo &) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<> populate(Vec<Sys::CpuInfo> &) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<> populate(Sys::UserInfo &) {
    return Error{Error::NOT_IMPLEMENTED};
}

Res<> populate(Vec<Sys::UserInfo> &) {
    return Error{Error::NOT_IMPLEMENTED};
}

} // namespace Embed
