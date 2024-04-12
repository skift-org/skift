#pragma once

#include <karm-base/reflect.h>
#include <karm-base/vec.h>

#include "bscan.h"

namespace Karm::Io {

template <typename T>
struct Packer;

template <typename T>
static constexpr void pack(BEmit &e, T const &val) {
    Packer<T>::pack(e, val);
}

template <typename T>
static constexpr T unpack(BScan &s) {
    return Packer<T>::unpack(s);
}

// MARK: Trivialy Copyable -----------------------------------------------------

template <Meta::TrivialyCopyable T>
struct Packer<T> {
    static void pack(BEmit &e, T const &val) {
        e.writeFrom(val);
    }

    static T unpack(BScan &s) {
        T res;
        s.readTo(&res);
        return res;
    }
};

// MARK: Optionals -------------------------------------------------------------

template <>
struct Packer<None> {
    static void pack(BEmit &, None const &) {
    }

    static None unpack(BScan &) {
        return NONE;
    }
};

template <typename T>
struct Packer<Opt<T>> {
    static void pack(BEmit &e, Opt<T> const &val) {
        e.writeU8le(val.has());
        if (val.has())
            pack(e, val.unwrap());
    }

    static Opt<T> unpack(BScan &s) {
        bool has = s.nextU8le();
        if (not has)
            return NONE;
        return unpack<T>();
    }
};

template <>
struct Packer<Error> {
    // TODO: Because the message message in the error is a non owning string
    //       we can't send it over the wire because their will be no one
    //       to own it at the other and.
    //
    //       This should be fine from a technical standpoint since the code
    //       don't care about the message, but the user does thougt.

    static void pack(BEmit &e, Error const &val) {
        Io::pack(e, (u32)val.code());
    }

    static Error unpack(BScan &s) {
        return {(Error::Code)Io::unpack<u32>(s), nullptr};
    }
};

template <typename T, typename E>
struct Packer<Res<T, E>> {
    static void pack(BEmit &e, Res<T, E> const &val) {
        e.writeU8le(val.has());
        if (val.has())
            Io::pack(e, val.unwrap());
        else
            Io::pack(e, val.none());
    }

    static Res<T, E> unpack(BScan &s) {
        bool has = s.nextU8le();
        if (has) {
            return Ok(Io::unpack<T>(s));
        }
        return Io::unpack<E>(s);
    }
};

// MARK: Reflectable ----------------------------------------------------------

template <Reflectable T>
struct Packer<T> {
    static void pack(BEmit &e, T const &val) {
        iterFields(val, [&](auto, auto const &v) {
            Io::pack(e, v);
        });
    }

    static T unpack(BScan &s) {
        T res;
        iterFields(res, [&](auto, auto const &v) {
            Io::unpack(s, v);
        });
        return res;
    }
};

// MARK: Sliceable ---------------------------------------------------------------

template <typename T>
struct Packer<Vec<T>> {
    static void pack(BEmit &e, Vec<T> const &val) {
        e.writeU64le(val.len());
        for (auto &i : val) {
            Io::pack(e, i);
        }
    }

    static void unpack(BScan &s) {
        auto len = s.nextU64le();
        Vec<T> res;
        res.ensure(len);
        for (usize i = 0; i < len; i++) {
            res.emplaceBack(Io::unpack<T>(s));
        }
        return res;
    }
};

// MARK: Strings ---------------------------------------------------------------

template <StaticEncoding E>
struct Packer<_String<E>> {
    static void pack(BEmit &e, _String<E> const &val) {
        e.writeU64le(val.len());
        e.writeStr(val);
    }

    static String unpack(BScan &s) {
        StringBuilder b;
        auto len = s.nextU64le();
        b.ensure(len);
        b.append(s.nextStr(len));
        return b.take();
    }
};

// MARK: Tuple -----------------------------------------------------------------

template <typename Car, typename Cdr>
struct Packer<Cons<Car, Cdr>> {
    static void pack(BEmit &e, Cons<Car, Cdr> const &val) {
        pack(e, val.car);
        pack(e, val.cdr);
    }

    static Cons<Car, Cdr> unpack(BScan &s) {
        return {
            Io::unpack<Car>(s),
            Io::unpack<Cdr>(s),
        };
    }
};

template <typename... Ts>
struct Packer<Tuple<Ts...>> {
    static void pack(BEmit &e, Tuple<Ts...> const &val) {
        val.visit([&](auto const &f) {
            Io::pack(e, f);
            return true;
        });
    }

    static Tuple<Ts...> unpack(BScan &s) {
        Tuple<Ts...> res;
        res.visit([&]<typename T>(T &f) {
            f = Io::unpack<T>(s);
            return true;
        });
        return res;
    }
};

} // namespace Karm::Io
