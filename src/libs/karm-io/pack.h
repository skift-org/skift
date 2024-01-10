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
static constexpr void unpack(BScan &s, T &val) {
    Packer<T>::unpack(s, val);
}

/* --- Trivialy Copyable ---------------------------------------------------- */

template <Meta::TrivialyCopyable T>
struct Packer<T> {
    static void pack(BEmit &e, T const &val) {
        e.writeFrom(val);
    }

    static void unpack(BScan &s, T &val) {
        s.readTo(&val);
    }
};

/* --- Optionals ------------------------------------------------------------ */

template <>
struct Packer<None> {
    static void pack(BEmit &, None const &) {
    }

    static void unpack(BScan &, None &) {
    }
};

template <typename T>
struct Packer<Opt<T>> {
    static void pack(BEmit &e, Opt<T> const &val) {
        e.writeU8le(val.has());
        if (val.has())
            pack(e, val.unwrap());
    }

    static void unpack(BScan &s, Opt<T> &val) {
        bool has = s.nextU8le();
        if (has)
            unpack(s, val.unwrap());
    }
};

template <typename T>
struct Packer<Ok<T>> {
    static void pack(BEmit &e, Ok<T> const &val) {
        pack(e, val.inner);
    }

    static void unpack(BScan &s, Ok<T> &val) {
        unpack(s, val.inner);
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

    static void unpack(BScan &s, Error &val) {
        Error::Code code;
        Io::unpack(s, code);
        val = code;
    }
};

template <typename T, typename E>
struct Packer<Res<T, E>> {
    static void pack(BEmit &e, Res<T, E> const &val) {
        e.writeU8le(val.has());
        if (val.has())
            pack(e, val.unwrap());
        else
            pack(e, val.none());
    }

    static void unpack(BScan &s, Res<T, E> &val) {
        bool has = s.nextU8le();
        if (has) {
            T res;
            unpack(s, res);
            val = Ok(std::move(res));
        } else {
            E err;
            unpack(s, err);
            val = err;
        }
    }
};

/* --- Reflectable --------------------------------------------------------- */

template <Reflectable T>
struct Packer<T> {
    static void pack(BEmit &e, T const &val) {
        iterFields(val, [&](auto, auto const &v) {
            pack(e, v);
        });
    }

    static void unpack(BScan &s, T &val) {
        iterFields(val, [&](auto, auto const &v) {
            unpack(s, v);
        });
    }
};

/* --- Sliceable -------------------------------------------------------------- */

template <typename T>
struct Packer<Vec<T>> {
    static void pack(BEmit &e, Vec<T> const &val) {
        e.writeU64le(val.len());
        for (auto &i : val) {
            pack(e, i);
        }
    }

    static void unpack(BScan &s, Vec<T> &val) {
        auto len = s.nextU64le();
        val.ensure();
        for (usize i = 0; i < len; i++) {
            unpack(s, val.emplaceBack());
        }
    }
};

/* --- Strings -------------------------------------------------------------- */

template <>
struct Packer<String> {
    static void pack(BEmit &e, String const &val) {
        e.writeU64le(val.len());
        e.writeStr(val);
    }

    static void unpack(BScan &s, String &val) {
        StringBuilder b;
        auto len = s.nextU64le();
        b.ensure(len);
        b.append(s.nextStr(len));
        val = b.take();
    }
};

/* --- Tuple ---------------------------------------------------------------- */

template <typename Car, typename Cdr>
struct Packer<Cons<Car, Cdr>> {
    static void pack(BEmit &e, Cons<Car, Cdr> const &val) {
        pack(e, val.car);
        pack(e, val.cdr);
    }

    static void unpack(BScan &s, Cons<Car, Cdr> &val) {
        unpack(s, val.car);
        unpack(s, val.cdr);
    }
};

template <typename... Ts>
struct Packer<Tuple<Ts...>> {
    static void pack(BEmit &e, Tuple<Ts...> const &val) {
        val.visit([&](auto const &f) {
            pack(e, f);
            return true;
        });
    }

    static void unpack(BScan &s, Tuple<Ts...> &val) {
        val.visit([&](auto &f) {
            unpack(s, f);
            return true;
        });
    }
};

} // namespace Karm::Io
