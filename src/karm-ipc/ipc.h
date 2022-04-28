#pragma once

#include <karm-base/macros.h>
#include <karm-base/pack.h>
#include <karm-io/types.h>

namespace Karm::Ipc {
using Id = uint64_t;

struct Header {
};

struct Message {
    struct Encoder {
        auto write(void const *, size_t size) -> Base::Result<size_t>;
        Message finalize();
    };

    struct Decoder {
        Decoder(Message &message);
        auto read(void *data, size_t size) -> Base::Result<size_t>;
    };
};

template <Id id, typename... Args>
struct Procedure {
    constexpr static Id ID = id;
    constexpr static bool do_return = false;

    auto encode(Io::Writer auto &writer, Args &&...args) -> Base::Result<size_t> {
        return Base::pack(writer, std::forward<Args>(args)...);
    }
};

template <Id id, typename Ret, typename... Args>
struct Function : public Procedure<id, Args...> {
    using Return = Ret;

    constexpr static bool do_return = true;

    auto decode(Io::Reader auto &reader, Ret &ret) -> Base::Result<size_t> {
        return Base::unpack(reader, ret);
    }
};

auto do_procedure_call(Message req) -> Base::Error;

auto do_function_call(Message req, Message &resp) -> Base::Error;

struct Endpoint {
};

struct Peer {
    static auto self() -> Peer &;

    void handle(Message &message);

    void run();
};

struct Object {
    using Handle = uint64_t;
    Handle handle;
};

struct Handle {
    uint64_t _value;

    template <typename Func, typename... Args>
    auto call(Args &&...args) -> Base::Result<typename Func::Return> {
        Func func;

        Message::Encoder encoder;
        func.encode(encoder, std::forward<Args>(args)...);

        if constexpr (Func::do_return) {
            Message response;
            try$(do_function_call(encoder.finalize(), response));

            Message::Decoder decoder{response};
            typename Func::Return ret;
            try$(func.decode(decoder, ret));

            return {ret};
        } else {
            return do_procedure_call(encoder.finalize());
        }
    }
};
} // namespace Karm::Ipc
