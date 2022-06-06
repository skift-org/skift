#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/traits.h>

#include "path.h"

namespace Karm::Sys {

struct Fd {
    virtual ~Fd() = default;

    virtual Result<size_t> read(void *buf, size_t size) = 0;

    virtual Result<size_t> write(void const *buf, size_t size) = 0;

    virtual Result<size_t> seek(Io::Seek seek) = 0;

    virtual Result<size_t> flush() = 0;

    virtual Result<Strong<Fd>> dup() = 0;
};

struct DummyFd : public Fd {
    Result<size_t> read(void *, size_t) override {
        notImplemented();
    }

    Result<size_t> write(void const *, size_t) override {
        notImplemented();
    }

    Result<size_t> seek(Io::Seek) override {
        notImplemented();
    }

    Result<size_t> flush() override {
        notImplemented();
    }

    Result<Strong<Fd>> dup() override {
        notImplemented();
    }
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Strong<Fd>>;
};

} // namespace Karm::Sys
