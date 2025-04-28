#pragma once

#include <karm-base/enum.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-meta/nocopy.h>
#include <karm-meta/visit.h>
#include <karm-sys/_handle.h>

#include "bscan.h"

namespace Karm::Io {

struct PackEmit : BEmit {
    Vec<Sys::Handle> _handles;

    using BEmit::BEmit;

    void give(Sys::Handle hnd) {
        _handles.emplaceBack(hnd);
    }

    Slice<Sys::Handle> handles() const {
        return _handles;
    }

    void clear() {
        _handles.clear();
    }
};

struct PackScan : BScan {
    Cursor<Sys::Handle> _handles;

    PackScan(Bytes bytes, Slice<Sys::Handle> handles)
        : BScan(bytes), _handles(handles) {}

    Sys::Handle take() {
        if (_handles.ended())
            return Sys::INVALID;
        return _handles.next();
    }
};

template <typename T>
struct Packer;

template <typename T>
static Res<> pack(PackEmit& e, T const& val) {
    return Packer<T>::pack(e, val);
}

template <typename T>
static Res<T> unpack(PackScan& s) {
    return Packer<T>::unpack(s);
}

// MARK: Trivialy Copyable -----------------------------------------------------

template <Meta::TrivialyCopyable T>
struct Packer<T> {
    static Res<> pack(PackEmit& e, T const& val) {
        e.writeFrom(val);
        return Ok();
    }

    static Res<T> unpack(PackScan& s) {
        T res;
        s.readTo(&res);
        return Ok(res);
    }
};

// MARK: Optionals -------------------------------------------------------------

template <>
struct Packer<None> {
    static Res<> pack(BEmit&, None const&) {
        return Ok();
    }

    static Res<None> unpack(PackScan&) {
        return NONE;
    }
};

template <typename T>
struct Packer<Opt<T>> {
    static Res<> pack(PackEmit& e, Opt<T> const& val) {
        e.writeU8le(val.has());
        if (val.has())
            try$(pack(e, val.unwrap()));
        return Ok();
    }

    static Res<Opt<T>> unpack(PackScan& s) {
        bool has = s.nextU8le();
        if (not has)
            return Ok<Opt<T>>(NONE);
        return Ok(unpack<T>());
    }
};

template <typename... Ts>
struct Packer<Union<Ts...>> {
    static Res<> pack(PackEmit& e, Union<Ts...> const& val) {
        try$(Io::pack<u8>(e, val.index()));
        return val.visit([&]<typename T>(T const& v) {
            try$(Io::pack<T>(e, v));
        });
    }

    static Res<Union<Ts...>> unpack(PackScan& s) {
        auto index = try$(Io::unpack<u8>(s));
        if (index >= sizeof...(Ts))
            return Error::invalidData("invalid union index");
        return Meta::indexCast<Ts...>(index, nullptr, [&]<typename T>(T*) -> Res<Union<Ts...>> {
            return Io::unpack<T>(s);
        });
    }
};

template <>
struct Packer<Error> {
    // TODO: Because the message in the error is a non owning string
    //       we can't send it over the wire because they will be no one
    //       to own it at the other end.
    //
    //       This should be fine from a technical standpoint since the code
    //       don't care about the message, but the user does though.

    static Res<> pack(PackEmit& e, Error const& val) {
        return Io::pack(e, (u32)val.code());
    }

    static Res<Error> unpack(PackScan& s) {
        auto code = (Error::Code)try$(Io::unpack<u32>(s));
        return Ok(Error{code, nullptr});
    }
};

template <typename T, typename E>
struct Packer<Res<T, E>> {
    static Res<> pack(PackEmit& e, Res<T, E> const& val) {
        e.writeU8le(val.has());
        if (val.has())
            return Io::pack(e, val.unwrap());
        return Io::pack(e, val.none());
    }

    static Res<Res<T, E>> unpack(PackScan& s) {
        bool has = s.nextU8le();
        if (has) {
            auto res = Ok<T>(try$(Io::unpack<T>(s)));
            return Ok<Res<T, E>>(std::move(res));
        }
        auto err = try$(Io::unpack<E>(s));
        return Ok<Res<T, E>>(err);
    }
};

template <Meta::Agregate T>
    requires(not Meta::TrivialyCopyable<T>)
struct Packer<T> {
    static Res<> pack(PackEmit& e, T const& val) {
        return Meta::visit(
            [&](auto&&... fields) {
                Res<> res = Ok();
                ([&] {
                    res ? res = Io::pack(e, fields) : res;
                }(),
                 ...);
                return res;
            },
            val
        );
    }

    static Res<T> unpack(PackScan& s) {
        T object;
        Opt<Error> err = NONE;
        Meta::visit(
            [&](auto&&... fields) {
                ([&] {
                    auto res = Io::unpack<Meta::RemoveConstVolatileRef<decltype(fields)>>(s);
                    if (not res) {
                        err = res.none();
                        return;
                    }
                    fields = res.take();
                }(),
                 ...);
            },
            object
        );

        if (err)
            return err.take();

        return Ok(object);
    }
};

// MARK: Sliceable ---------------------------------------------------------------

template <typename T>
struct Packer<Vec<T>> {
    static Res<> pack(PackEmit& e, Vec<T> const& val) {
        e.writeU64le(val.len());
        for (auto& i : val) {
            try$(Io::pack(e, i));
        }
        return Ok();
    }

    static Res<Vec<T>> unpack(PackScan& s) {
        auto len = s.nextU64le();
        Vec<T> res{len};
        for (usize i = 0; i < len; i++) {
            res.emplaceBack(try$(Io::unpack<T>(s)));
        }
        return Ok(std::move(res));
    }
};

// MARK: Strings ---------------------------------------------------------------

template <StaticEncoding E>
struct Packer<_String<E>> {
    static Res<> pack(PackEmit& e, _String<E> const& val) {
        e.writeU64le(val.len());
        e.writeStr(val);
        return Ok();
    }

    static Res<String> unpack(PackScan& s) {
        return Ok(s.nextStr(s.nextU64le()));
    }
};

// MARK: Tuple -----------------------------------------------------------------

template <typename Car, typename Cdr>
struct Packer<Pair<Car, Cdr>> {
    static Res<> pack(PackEmit& e, Pair<Car, Cdr> const& val) {
        Io::pack(e, val.v0);
        Io::pack(e, val.v1);
        return Ok();
    }

    static Res<Pair<Car, Cdr>> unpack(PackScan& s) {
        Pair res = {
            try$(Io::unpack<Car>(s)),
            try$(Io::unpack<Cdr>(s)),
        };
        return Ok(res);
    }
};

template <typename... Ts>
struct Packer<Tuple<Ts...>> {
    static Res<> pack(PackEmit& e, Tuple<Ts...> const& val) {
        return val.visit([&](auto const& f) {
            return Io::pack(e, f);
        });
    }

    static Res<Tuple<Ts...>> unpack(PackScan& s) {
        Tuple<Ts...> res;
        try$(res.visit([&]<typename T>(T& f) -> Res<> {
            f = try$(Io::unpack<T>(s));
            return Ok();
        }));
        return Ok(res);
    }
};

} // namespace Karm::Io
