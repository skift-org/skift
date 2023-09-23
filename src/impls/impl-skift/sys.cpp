#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>

#include <karm-sys/_embed.h>

namespace Karm::Sys::_Embed {

/* --- File I/O ------------------------------------------------------------- */

struct VmoFd : public Sys::Fd {
    Hj::Vmo _vmo;

    Hj::Vmo &vmo() {
        return _vmo;
    }

    VmoFd(Hj::Vmo vmo)
        : _vmo(std::move(vmo)) {}

    Res<usize> read(MutBytes) override {
        notImplemented();
    }

    Res<usize> write(Bytes) override {
        notImplemented();
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
};

Res<Strong<Sys::Fd>> openFile(Url::Url url) {
    auto urlStr = try$(url.str());
    auto *fileRecord = useHandover().fileByName(urlStr.buf());
    if (not fileRecord)
        return Error::invalidFilename();
    auto vmo = try$(Hj::Vmo::create(Hj::ROOT, fileRecord->start, fileRecord->size, Hj::VmoFlags::DMA));
    try$(vmo.label(urlStr));

    return Ok(makeStrong<VmoFd>(std::move(vmo)));
}

Res<Strong<Sys::Fd>> createFile(Url::Url) {
    notImplemented();
}

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe() {
    notImplemented();
}

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Vec<Sys::DirEntry>> readDir(Url::Url) {
    notImplemented();
}

/* --- Time ----------------------------------------------------------------- */

TimeStamp now() {
    return Hj::now().unwrap();
}

TimeSpan uptime() {
    notImplemented();
}

/* --- Memory Managment ----------------------------------------------------- */

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &) {
    notImplemented();
}

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &, Strong<Sys::Fd> fd) {
    if (fd.is<VmoFd>()) {
        auto &vmo = try$(fd.cast<VmoFd>())->vmo();
        auto range = try$(Hj::Space::self().map(vmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

        return Ok(Sys::MmapResult{
            0,
            range.start,
            range.size,
        });
    } else {
        return Error::invalidInput();
    }
}

Res<> memUnmap(void const *ptr, usize size) {
    return Hj::Space::self().unmap({(usize)ptr, size});
}

Res<> memFlush(void *, usize) {
    notImplemented();
}

/* --- System Informations -------------------------------------------------- */

Res<> populate(Sys::SysInfo &) {
    notImplemented();
}

Res<> populate(Sys::MemInfo &) {
    notImplemented();
}

Res<> populate(Vec<Sys::CpuInfo> &) {
    notImplemented();
}

Res<> populate(Sys::UserInfo &) {
    notImplemented();
}

Res<> populate(Vec<Sys::UserInfo> &) {
    notImplemented();
}

/* --- Process Managment ---------------------------------------------------- */

Res<> sleep(TimeSpan) {
    notImplemented();
}

Res<> exit(i32) {
    notImplemented();
}

} // namespace Karm::Sys::_Embed
