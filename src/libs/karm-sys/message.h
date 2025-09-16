#pragma once

import Karm.Core;

#include <karm-core/macros.h>
#include <karm-sys/types.h>

namespace Karm::Sys {

// MARK: Message Writer/Reader -------------------------------------------------

struct MessageWriter : Io::BEmit {
    Vec<Handle> _handles;

    using BEmit::BEmit;

    void give(Handle hnd) {
        _handles.emplaceBack(hnd);
    }

    Slice<Handle> handles() const {
        return _handles;
    }

    void clear() {
        _handles.clear();
    }
};

struct MessageReader : Io::BScan {
    Cursor<Handle> _handles;

    MessageReader(Bytes bytes, Slice<Handle> handles)
        : BScan(bytes), _handles(handles) {}

    Handle take() {
        if (_handles.ended())
            return INVALID;
        return _handles.next();
    }
};

template <typename T>
struct MessagePacker;

template <typename T>
static Res<> pack(MessageWriter& e, T const& val) {
    return MessagePacker<T>::pack(e, val);
}

template <typename T>
static Res<T> unpack(MessageReader& s) {
    return MessagePacker<T>::unpack(s);
}

// MARK: Primitive Types -------------------------------------------------------

struct Port : Distinct<u64, struct _PortTag> {
    static Port const INVALID;
    static Port const BUS;
    static Port const BROADCAST;

    using Distinct::Distinct;

    void repr(Io::Emit& e) const {
        if (*this == INVALID)
            e("invalid");
        else if (*this == BUS)
            e("bus");
        else if (*this == BROADCAST)
            e("broadcast");
        else
            e("{}", value());
    }
};

constexpr Port Port::INVALID{0};
constexpr Port Port::BUS{Limits<u64>::MAX};
constexpr Port Port::BROADCAST{Limits<u64>::MAX - 1};

struct Header {
    u64 seq;
    Port from;
    Port to;
    Meta::Id mid;

    void repr(Io::Emit& e) const {
        e("(header seq: {}, from: {}, to: {}, mid: {:016x})", seq, from, to, mid);
    }
};

static_assert(Meta::TrivialyCopyable<Header>);

struct Message {
    static constexpr usize CAP = 4096;

    union {
        struct {
            Header _header;
            Array<u8, CAP - sizeof(Header)> _payload;
        };

        Array<u8, CAP> _buf;
    };

    usize _len = 0;

    Array<Handle, 16> _hnds;
    usize _hndsLen = 0;

    Header& header() {
        return _header;
    }

    Header const& header() const {
        return _header;
    }

    usize len() const {
        return _len;
    }

    Bytes bytes() {
        return sub(_buf, 0, len());
    }

    Slice<Handle> handles() {
        return sub(_hnds, 0, _hndsLen);
    }

    template <typename T>
    bool is() const {
        return _header.mid == Meta::idOf<T>();
    }

    template <typename T, typename... Args>
    static Res<Message> packReq(Port to, u64 seq, Args&&... args) {
        T payload{std::forward<Args>(args)...};

        Message msg;
        msg._header = {
            seq,
            Port::INVALID,
            to,
            Meta::idOf<T>(),
        };
        Io::BufWriter reqBuf{msg._payload};
        MessageWriter reqPack{reqBuf};

        try$(Sys::pack(reqPack, payload));

        msg._len = try$(Io::tell(reqBuf)) + sizeof(Header);

        return Ok(std::move(msg));
    }

    template <typename T, typename... Args>
    Res<Message> packResp(Args&&... args) {
        typename T::Response payload{std::forward<Args>(args)...};

        Message resp;
        resp._header = {
            header().seq,
            header().to,
            header().from,
            Meta::idOf<typename T::Response>(),
        };

        Io::BufWriter respBuf{resp._payload};
        MessageWriter respPack{respBuf};

        try$(Sys::pack(respPack, payload));

        resp._len = try$(Io::tell(respBuf)) + sizeof(Header);

        return Ok(std::move(resp));
    }

    template <typename T>
    Res<T> unpack() {
        MessageReader s{bytes(), handles()};
        if (not is<T>())
            return Error::invalidData("unexpected message");
        try$(Sys::unpack<Header>(s));
        return Sys::unpack<T>(s);
    }
};

// MARK: Trivialy Copyable -----------------------------------------------------

template <Meta::TrivialyCopyable T>
struct MessagePacker<T> {
    static Res<> pack(MessageWriter& e, T const& val) {
        e.writeFrom(val);
        return Ok();
    }

    static Res<T> unpack(MessageReader& s) {
        T res;
        s.readTo(&res);
        return Ok(res);
    }
};

// MARK: Optionals -------------------------------------------------------------

template <>
struct MessagePacker<None> {
    static Res<> pack(Io::BEmit&, None const&) {
        return Ok();
    }

    static Res<None> unpack(MessageReader&) {
        return NONE;
    }
};

template <typename T>
struct MessagePacker<Opt<T>> {
    static Res<> pack(MessageWriter& e, Opt<T> const& val) {
        e.writeU8le(val.has());
        if (val.has())
            try$(pack(e, val.unwrap()));
        return Ok();
    }

    static Res<Opt<T>> unpack(MessageReader& s) {
        bool has = s.nextU8le();
        if (not has)
            return Ok<Opt<T>>(NONE);
        return Ok(unpack<T>());
    }
};

template <typename... Ts>
struct MessagePacker<Union<Ts...>> {
    static Res<> pack(MessageWriter& e, Union<Ts...> const& val) {
        try$(Sys::pack<u8>(e, val.index()));
        return val.visit([&]<typename T>(T const& v) {
            try$(Sys::pack<T>(e, v));
        });
    }

    static Res<Union<Ts...>> unpack(MessageReader& s) {
        auto index = try$(Sys::unpack<u8>(s));
        if (index >= sizeof...(Ts))
            return Error::invalidData("invalid union index");
        return Meta::indexCast<Ts...>(index, nullptr, [&]<typename T>(T*) -> Res<Union<Ts...>> {
            return Sys::unpack<T>(s);
        });
    }
};

template <>
struct MessagePacker<Error> {
    // TODO: Because the message in the error is a non owning string
    //       we can't send it over the wire because they will be no one
    //       to own it at the other end.
    //
    //       This should be fine from a technical standpoint since the code
    //       don't care about the message, but the user does though.

    static Res<> pack(MessageWriter& e, Error const& val) {
        return Sys::pack(e, static_cast<u32>(val.code()));
    }

    static Res<Error> unpack(MessageReader& s) {
        auto code = (Error::Code)try$(Sys::unpack<u32>(s));
        return Ok(Error{code, nullptr});
    }
};

template <typename T, typename E>
struct MessagePacker<Res<T, E>> {
    static Res<> pack(MessageWriter& e, Res<T, E> const& val) {
        e.writeU8le(val.has());
        if (val.has())
            return Sys::pack(e, val.unwrap());
        return Sys::pack(e, val.none());
    }

    static Res<Res<T, E>> unpack(MessageReader& s) {
        bool has = s.nextU8le();
        if (has) {
            auto res = Ok<T>(try$(Sys::unpack<T>(s)));
            return Ok<Res<T, E>>(std::move(res));
        }
        auto err = try$(Sys::unpack<E>(s));
        return Ok<Res<T, E>>(err);
    }
};

template <Meta::Aggregate T>
    requires(not Meta::TrivialyCopyable<T>)
struct MessagePacker<T> {
    static Res<> pack(MessageWriter& e, T const& val) {
        return Meta::visit(
            val,
            [&](auto&&... fields) {
                Res<> res = Ok();
                ([&] {
                    res ? res = Sys::pack(e, fields) : res;
                }(),
                 ...);
                return res;
            }
        );
    }

    static Res<T> unpack(MessageReader& s) {
        T object;
        Opt<Error> err = NONE;
        Meta::visit(
            object,
            [&](auto&&... fields) {
                ([&] {
                    auto res = Sys::unpack<Meta::RemoveConstVolatileRef<decltype(fields)>>(s);
                    if (not res) {
                        err = res.none();
                        return;
                    }
                    fields = res.take();
                }(),
                 ...);
            }
        );

        if (err)
            return err.take();

        return Ok(object);
    }
};

// MARK: Sliceable ---------------------------------------------------------------

template <typename T>
struct MessagePacker<Vec<T>> {
    static Res<> pack(MessageWriter& e, Vec<T> const& val) {
        e.writeU64le(val.len());
        for (auto& i : val) {
            try$(Sys::pack(e, i));
        }
        return Ok();
    }

    static Res<Vec<T>> unpack(MessageReader& s) {
        auto len = s.nextU64le();
        Vec<T> res{len};
        for (usize i = 0; i < len; i++) {
            res.emplaceBack(try$(Sys::unpack<T>(s)));
        }
        return Ok(std::move(res));
    }
};

// MARK: Strings ---------------------------------------------------------------

template <StaticEncoding E>
struct MessagePacker<_String<E>> {
    static Res<> pack(MessageWriter& e, _String<E> const& val) {
        e.writeU64le(val.len());
        e.writeStr(val);
        return Ok();
    }

    static Res<String> unpack(MessageReader& s) {
        return Ok(s.nextStr(s.nextU64le()));
    }
};

// MARK: Tuple -----------------------------------------------------------------

template <typename Car, typename Cdr>
struct MessagePacker<Pair<Car, Cdr>> {
    static Res<> pack(MessageWriter& e, Pair<Car, Cdr> const& val) {
        Sys::pack(e, val.v0);
        Sys::pack(e, val.v1);
        return Ok();
    }

    static Res<Pair<Car, Cdr>> unpack(MessageReader& s) {
        Pair res = {
            try$(Sys::unpack<Car>(s)),
            try$(Sys::unpack<Cdr>(s)),
        };
        return Ok(res);
    }
};

template <typename... Ts>
struct MessagePacker<Tuple<Ts...>> {
    static Res<> pack(MessageWriter& e, Tuple<Ts...> const& val) {
        return val.visit([&](auto const& f) {
            return Sys::pack(e, f);
        });
    }

    static Res<Tuple<Ts...>> unpack(MessageReader& s) {
        Tuple<Ts...> res;
        try$(res.visit([&]<typename T>(T& f) -> Res<> {
            f = try$(Sys::unpack<T>(s));
            return Ok();
        }));
        return Ok(res);
    }
};

} // namespace Karm::Sys
