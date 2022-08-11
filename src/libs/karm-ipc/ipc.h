#pragma once

#include <karm-base/pack.h>
#include <karm-base/try.h>
#include <karm-io/types.h>

namespace Karm::Ipc {

using Id = uint64_t;

struct Header {
};

struct Message {
    struct Encoder {
        Result<size_t> write(void const *, size_t size);
        Message finalize();
    };

    struct Decoder {
        Decoder(Message &message);
        Result<size_t> read(void *data, size_t size);
    };
};

template <Id id, typename... Args>
struct Procedure {
    constexpr static Id ID = id;
    constexpr static bool do_return = false;

    Result<size_t> encode(Io::Writable auto &writer, Args &&...args) {
        return pack(writer, std::forward<Args>(args)...);
    }
};

template <Id id, typename _Ret, typename... Args>
struct Function : public Procedure<id, Args...> {
    using Ret = _Ret;

    constexpr static bool do_return = true;

    Result<size_t> decode(Io::Readable auto &reader, Ret &ret) {
        return unpack(reader, ret);
    }
};

Error doProcedureCall(Message req);

Error doFunctionCall(Message req, Message &resp);

struct Endpoint {
};

struct Peer {
    static Peer &self();

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
    Result<typename Func::Ret> call(Args &&...args) {
        Func func;

        Message::Encoder encoder;
        func.encode(encoder, std::forward<Args>(args)...);

        if constexpr (Func::do_return) {
            Message response;
            try$(doFunctionCall(encoder.finalize(), response));

            Message::Decoder decoder{response};
            typename Func::Ret ret;
            try$(func.decode(decoder, ret));

            return {ret};
        } else {
            return doProcedureCall(encoder.finalize());
        }
    }
};

} // namespace Karm::Ipc
