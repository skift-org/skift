#include <karm-base/func.h>
#include <karm-base/res.h>
#include <karm-base/time.h>
#include <karm-logger/logger.h>
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

    Sys::Handle handle() const override {
        return Handle{(usize)_proto};
    }

    Res<usize> read(MutBytes) override {
        notImplemented();
    }

    Res<usize> write(Bytes bytes) override {
        return _buf.write(bytes);
    }

    Res<usize> seek(Io::Seek) override {
        notImplemented();
    }

    Res<> flush() override {
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

SystemTime now() {
    auto span = embedGetTimeStamp();
    return SystemTime::epoch() + Duration::fromMSecs(span);
}

Res<Rc<Sys::Fd>> createIn() {
    return Ok(makeRc<Sys::NullFd>());
}

Res<Rc<Sys::Fd>> createOut() {
    return Ok(makeRc<JSConsole>(JSConsole::LOG));
}

Res<Rc<Sys::Fd>> createErr() {
    return Ok(makeRc<JSConsole>(JSConsole::ERROR));
}

Res<Rc<Sys::Fd>> unpackFd(Io::PackScan&) {
    notImplemented();
}

Res<Vec<DirEntry>> readDir(Mime::Url const&) {
    return Error::notImplemented("directory listing not supported");
}

Res<Stat> stat(Mime::Url const&) {
    return Error::notImplemented("directory listing not supported");
}

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Fd>> openFile(Mime::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> createFile(Mime::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> openOrCreateFile(Mime::Url const&) {
    return Error::notImplemented();
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Fd>> listenUdp(SocketAddr) {
    return Error::notImplemented("raw sockets not supported");
}

Res<Rc<Fd>> connectTcp(SocketAddr) {
    return Error::notImplemented("raw sockets not supported");
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    return Error::notImplemented("raw sockets not supported");
}

Res<Rc<Fd>> listenIpc(Mime::Url) {
    return Error::notImplemented("ipc sockets not supported");
}

// MARK: Memory Managment ------------------------------------------------------

Res<MmapResult> memMap(MmapOptions const&, Rc<Fd>) {
    return Error::notImplemented("file mapping not supported");
}

Res<> memUnmap(void const*, usize) {
    return Error::notImplemented();
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo&) {
    return Ok();
}

Res<> populate(Sys::MemInfo& mem) {
    mem.physicalTotal = (usize)__heap_end - (usize)__heap_base;
    mem.physicalUsed = -1;
    mem.swapTotal = 0;
    mem.swapUsed = 0;
    mem.virtualTotal = 0;
    mem.virtualUsed = 0;
    return Ok();
}

Res<> populate(Vec<Sys::CpuInfo>&) {
    return Ok();
}

Res<> populate(Sys::UserInfo&) {
    return Ok();
}

Res<> populate(Vec<Sys::UserInfo>&) {
    return Ok();
}

// MARK: Sandboxing ------------------------------------------------------------

void hardenSandbox() {
    logError("could not harden sandbox");
}

} // namespace Karm::Sys::_Embed
