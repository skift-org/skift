#pragma once

import Karm.Core;
import Karm.Ref;

#include "addr.h"
#include "message.h"
#include "stat.h"
#include "types.h"

namespace Karm::Sys {

struct Fd;

using _Accepted = Pair<Rc<Fd>, SocketAddr>;
using _Sent = Pair<usize, usize>;
using _Received = Tuple<usize, usize, SocketAddr>;

struct Fd : Meta::NoCopy {
    virtual ~Fd() = default;

    virtual Handle handle() const = 0;

    virtual Res<usize> read(MutBytes) = 0;

    virtual Res<usize> write(Bytes) = 0;

    virtual Res<usize> seek(Io::Seek) = 0;

    virtual Res<> flush() = 0;

    virtual Res<Rc<Fd>> dup() = 0;

    virtual Res<_Accepted> accept() = 0;

    virtual Res<Stat> stat() = 0;

    virtual Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) = 0;

    virtual Res<_Received> recv(MutBytes, MutSlice<Handle>) = 0;

    virtual Res<> pack(MessageWriter& e) = 0;

    static Res<Rc<Fd>> unpack(MessageReader& s);
};

struct BlobFd : Fd {
    Rc<Ref::Blob> _blob;
    usize _offset;

    BlobFd(Rc<Ref::Blob> const& blob)
        : _blob(blob) {}

    Handle handle() const override {
        return INVALID;
    }

    Res<usize> read(MutBytes bytes) override {
        auto src = sub(_blob->data, _offset, _offset + bytes.len());
        return Ok(copy(src, bytes));
    }

    Res<usize> write(Bytes) override {
        return Error::readOnlyFilesystem();
    }

    Res<usize> seek(Io::Seek seek) override {
        _offset = seek.apply(_offset, _blob->len());
        return Ok(_offset);
    }

    Res<> flush() override {
        return Ok();
    }

    Res<Rc<Fd>> dup() override {
        return Ok(makeRc<BlobFd>(_blob));
    }

    Res<_Accepted> accept() override {
        return Error::notImplemented();
    }

    Res<Stat> stat() override {
        return Ok(Stat{
            .type = Type::FILE,
            .size = _blob->data.len(),
            .accessTime = SystemTime::END_OF_TIME,
            .modifyTime = SystemTime::END_OF_TIME,
            .changeTime = SystemTime::END_OF_TIME,
        });
    }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        return Error::notImplemented();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        return Error::notImplemented();
    }

    Res<> pack(MessageWriter&) override {
        return Error::notImplemented();
    }
};

struct NullFd : Fd {
    Handle handle() const override {
        return INVALID;
    }

    Res<usize> read(MutBytes) override;

    Res<usize> write(Bytes) override;

    Res<usize> seek(Io::Seek) override;

    Res<> flush() override;

    Res<Rc<Fd>> dup() override;

    Res<_Accepted> accept() override;

    Res<Stat> stat() override;

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override;

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override;

    Res<> pack(MessageWriter& e) override;
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Rc<Fd>>;
};

} // namespace Karm::Sys
