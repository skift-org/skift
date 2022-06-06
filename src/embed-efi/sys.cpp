#include <efi/base.h>
#include <embed/sys.h>

namespace Embed {

struct ConOut : public Sys::Fd {
    Efi::SimpleTextOutputProtocol *_proto;

    ConOut(Efi::SimpleTextOutputProtocol *proto) : _proto(proto) {}

    Result<size_t> read(void *, size_t) override {
        notImplemented();
    }

    Result<size_t> write(void const *buf, size_t size) override {
        Str s = {(char const *)buf, size};
        auto str = String{s}.transcoded<Utf16>();
        try$(_proto->outputString(_proto, str.buf()));
        return size;
    }

    Result<size_t> seek(Io::Seek) override {
        notImplemented();
    }

    Result<size_t> flush() override {
        /* NOTE: No-op */
        return 0;
    }

    Result<Strong<Fd>> dup() override {
        notImplemented();
    }
};

Result<Strong<Sys::Fd>> createIn() {
    return {makeStrong<Sys::DummyFd>()};
}

Result<Strong<Sys::Fd>> createOut() {
    return {makeStrong<ConOut>(Efi::st()->conOut)};
}

Result<Strong<Sys::Fd>> createErr() {
    return {makeStrong<ConOut>(Efi::st()->stdErr)};
}

} // namespace Embed
