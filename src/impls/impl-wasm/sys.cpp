#include <karm-base/std.h>
#include <karm-crypto/base64.h>
#include <karm-logger/logger.h>
#include <karm-mime/path.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <web-json/json.h>

#include <karm-sys/_embed.h>

#include "externs.h"

namespace Karm::Sys::_Embed {
struct JSConsole : public Sys::Fd {
    enum Proto {
        LOG,
        ERROR
    } _proto;

    Io::BufferWriter _buf;
    JSConsole(Proto proto) : _proto(proto) {}

    Sys::Handle handle() const override { return Handle{(usize)_proto}; }
    Res<usize> read(MutBytes) override { notImplemented(); }

    Res<usize> write(Bytes bytes) override { return _buf.write(bytes); }

    Res<usize> seek(Io::Seek) override { notImplemented(); }

    Res<usize> flush() override {
        switch (_proto) {
        case LOG:
            embedConsoleLog(_buf.bytes().buf(), _buf.bytes().len());
            break;
        case ERROR:
            embedConsoleError(_buf.bytes().buf(), _buf.bytes().len());
            break;
        }

        return _buf.flush();
    }

    Res<Strong<Fd>> dup() override { notImplemented(); }

    Res<_Accepted> accept() override { notImplemented(); }

    Res<Stat> stat() override { return Ok<Stat>(); }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override { notImplemented(); }

    Res<> pack(Io::PackEmit &) override { notImplemented(); }
};

TimeStamp now() {
    auto span = embedGetTimeStamp();
    return TimeStamp::epoch() + TimeSpan::fromMSecs(span);
}

Res<Strong<Sys::Fd>> createIn() { return Ok(makeStrong<Sys::NullFd>()); }

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<JSConsole>(JSConsole::LOG));
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<JSConsole>(JSConsole::ERROR));
}

Res<Strong<Sys::Fd>> unpackFd(Io::PackScan &) { notImplemented(); }

// MARK: Files -----------------------------------------------------------------

struct WasmFile : public Fd {
    String _path;
    usize _len;
    Buf<Byte> _buf;
    Io::BufReader _reader;

    static Res<Strong<Fd>> open(String path) {
        usize const buf_len = embedGetEstimedFileSize((u8 const *)path.buf(), path.len());
        auto buf = Buf<Byte>::init(buf_len, 0x00);
        // embedOpenFile((u8 const *)path.buf(), path.len(), (u8 *)buf.buf(),
        // buf.len());

        u64 id = embedOpenFile((u8 const *)path.buf(), path.len());
        embedCheckAsync(id, buf.buf(), buf.len());

        // logInfo("{}", buf);

        return Ok(makeStrong<WasmFile>(path, std::move(buf)));
    }

    WasmFile(String path, Buf<Byte> buf)
        : _path(path), _buf(std::move(buf)), _reader(_buf) {}

    Handle handle() const override { return Handle{0}; }

    Res<usize> read(MutBytes bytes) override { return _reader.read(bytes); }

    Res<usize> write(Bytes) override { return Error::notImplemented("write"); }

    Res<usize> seek(Io::Seek seek) override { return _reader.seek(seek); }

    Res<usize> flush() override { return Error::notImplemented("flush"); }

    Res<Strong<Fd>> dup() override { return Error::notImplemented("dup"); }

    Res<_Accepted> accept() override { return Error::notImplemented("accept"); }

    Res<Stat> stat() override { return Error::notImplemented("stat"); }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        return Error::notImplemented("send");
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        return Error::notImplemented("recv");
    }

    Res<> pack(Io::PackEmit &) override { return Error::notImplemented("pack"); }
};

Res<Strong<Sys::Fd>> openFile(Mime::Url const &url) {
    if (!(url.scheme == "bundle")) {
        return Error::invalidFilename("not-a-bundle-file");
    }

    return Ok(try$(WasmFile::open(url.str())));
}

Res<Strong<Fd>> createFile(Mime::Url const &) {
    return Error::notImplemented("createFile");
}

Res<Strong<Fd>> openOrCreateFile(Mime::Url const &) {
    return Error::notImplemented("openOrCreateFile");
}

Res<MmapResult> memMap(MmapOptions const &, Strong<Fd> fd) {
    usize fileSize = try$(Io::size(*fd));
    usize ptr = (usize)malloc(fileSize);

    MutBytes bytes = {(Byte *)ptr, fileSize};
    Io::BufWriter writer{bytes};
    try$(Io::copy(*fd, writer));

    return Ok(MmapResult{ptr, ptr, fileSize});
}

Res<> memUnmap(void const *ptr, usize) {
    free((void *)ptr);
    return Ok();
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo &) { return Ok(); }

Res<> populate(Sys::MemInfo &mem) {
    mem.physicalTotal = (usize)__heap_end - (usize)__heap_base;
    mem.physicalUsed = -1;
    mem.swapTotal = 0;
    mem.swapUsed = 0;
    mem.virtualTotal = 0;
    mem.virtualUsed = 0;
    return Ok();
}

Res<> populate(Vec<Sys::CpuInfo> &) { return Ok(); }

Res<> populate(Sys::UserInfo &) { return Ok(); }

Res<> populate(Vec<Sys::UserInfo> &) { return Ok(); }
} // namespace Karm::Sys::_Embed