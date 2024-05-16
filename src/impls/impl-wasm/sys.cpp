#include <karm-sys/_embed.h>
#include <karm-base/res.h>
#include <karm-base/func.h>
#include <karm-base/time.h>

#include "externs.h"


namespace Karm::Sys::_Embed {
    struct JSConsole : public Sys::Fd {
        enum Proto {
            Log,
            Error
        } _proto;

        JSConsole(Proto proto) : _proto(proto) {}

        Sys::Handle handle() const override {
            return Handle{(usize)0};
        }
        Res<usize> read(MutBytes) override {
            notImplemented();
        }

        Res<usize> write(Bytes bytes) override {
            switch (_proto) {
                case Log:
                    embedConsoleLog(bytes._buf, bytes._len);
                    break;
                case Error:
                    embedConsoleError(bytes._buf, bytes._len);
                    break;
            }

            return Ok(bytes._len);
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

    TimeStamp now() {
        auto span = embedGetTimeStamp();
        return TimeStamp::epoch() + TimeSpan::fromMSecs(span);
    }

    Res<Strong<Sys::Fd>> createIn() {
        return Ok(makeStrong<Sys::NullFd>());
    }

    Res<Strong<Sys::Fd>> createOut() {
        return Ok(makeStrong<JSConsole>(JSConsole::Log));
    }

    Res<Strong<Sys::Fd>> createErr() {
        return Ok(makeStrong<JSConsole>(JSConsole::Error));
    }

    Res<Strong<Sys::Fd>> unpackFd(Io::PackScan &) {
        notImplemented();
    }
} // namespace Karm::Sys::_Embed