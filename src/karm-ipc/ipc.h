#pragma once

#include <karm-base/macros.h>
#include <karm-base/pack.h>
#include <karm-io/types.h>

namespace Karm::Ipc
{
using Id = uint64_t;

struct Header
{
};

struct Message
{
    struct Encoder : Io::Writer
    {
        Io::Result write(void const *, size_t size);
        Message finalize();
    };

    struct Decoder : Io::Reader
    {
        Decoder(Message &message);
        Io::Result read(void *data, size_t size);
    };
};

template <Id id, typename... Args>
struct Procedure
{
    constexpr static Id ID = id;
    constexpr static bool do_return = false;

    void encode(Io::Writer &writer, Args &&...args)
    {
        (Base::pack(writer, std::forward<Args>(args)), ...);
    }
};

template <Id id, typename Ret, typename... Args>
struct Function : public Procedure<id, Args...>
{
    using Return = Ret;

    constexpr static bool do_return = true;

    Io::Result decode(Io::Reader &reader, Ret &ret)
    {
        return Base::unpack(reader, ret);
    }
};

Base::Error do_procedure_call(Message req);

Base::Error do_function_call(Message req, Message &resp);

struct Endpoint
{
};

struct Peer
{
    static Peer &self();

    void handle(Message &message);

    void run();
};

struct Object
{
    using Handle = uint64_t;
    Handle handle;
};

struct Handle
{
    uint64_t _value;

    template <typename Func, typename... Args>
    Base::Result<typename Func::Return> call(Args &&...args)
    {
        Func func;

        Message::Encoder encoder;
        func.encode(encoder, std::forward<Args>(args)...);

        if constexpr (Func::do_return)
        {
            Message response;
            try$(do_function_call(encoder.finalize(), response));

            Message::Decoder decoder{response};
            typename Func::Return ret;
            try$(func.decode(decoder, ret));

            return {ret};
        }
        else
        {
            return do_procedure_call(encoder.finalize());
        }
    }
};
} // namespace Karm::Ipc
