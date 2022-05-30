#pragma once

#include <karm-meta/traits.h>

#include "_prelude.h"

#include "array.h"
#include "cursor.h"

namespace Karm {

using Rune = uint32_t;

template <typename T>
concept Encoding = requires(T t, Rune &r, typename T::Unit u, Cursor<typename T::Unit> &c, MutCursor<typename T::Unit> &m) {
    { T::len(u) } -> Meta::Same<size_t>;
    { T::decode(r, c) } -> Meta::Same<bool>;
    { T::encode(Rune{}, m) } -> Meta::Same<bool>;
};

/* --- Utf8 ----------------------------------------------------------------- */

struct Utf8 {
    using Unit = char;
    static constexpr size_t MULTI_BYTE_LEN = 4;

    static constexpr size_t len(Unit first) {
        if ((first & 0xf8) == 0xf0)
            return 4;
        else if ((first & 0xf0) == 0xe0)
            return 3;
        else if ((first & 0xe0) == 0xc0)
            return 2;
        else
            return 1;
    }

    static bool decode(Rune &result, Cursor<Unit> &in) {
        if (in.rem() == 0) {
            result = U'�';
            return false;
        }

        Unit first = in.next();

        if (len(first) > in.rem() + 1) {
            return false;
        }

        if ((first & 0xf8) == 0xf0) {
            result = ((0x07 & first) << 18) |
                     ((0x3f & in.next()) << 12) |
                     ((0x3f & in.next()) << 6) |
                     ((0x3f & in.next()));
        } else if ((first & 0xf0) == 0xe0) {
            result = ((0x0f & first) << 12) |
                     ((0x3f & in.next()) << 6) |
                     ((0x3f & in.next()));
        } else if ((first & 0xe0) == 0xc0) {
            result = ((0x1f & first) << 6) |
                     ((0x3f & in.next()));
        } else {
            result = first;
        }

        return true;
    }

    static bool encode(Rune c, MutCursor<Unit> &out) {
        if (c <= 0x7f) {
            out.put(c);
        } else if (c <= 0x7ff) {
            out.put(0xc0 | (c >> 6));
            out.put(0x80 | (c & 0x3f));
        } else if (c <= 0xffff) {
            out.put(0xe0 | (c >> 12));
            out.put(0x80 | ((c >> 6) & 0x3f));
            out.put(0x80 | (c & 0x3f));
        } else if (c <= 0x10ffff) {
            out.put(0xf0 | (c >> 18));
            out.put(0x80 | ((c >> 12) & 0x3f));
            out.put(0x80 | ((c >> 6) & 0x3f));
            out.put(0x80 | (c & 0x3f));
        } else {
            encode(U'�', out);
            return false;
        }

        return true;
    }
};

[[gnu::used]] static inline Utf8 UTF8;

static_assert(Encoding<Utf8>);

/* --- Utf16 ---------------------------------------------------------------- */

struct Utf16 {
    using Unit = uint16_t;

    static constexpr size_t len(Unit first) {
        if (first >= 0xd800 && first <= 0xdbff)
            return 2;
        else
            return 1;
    }

    static bool decode(Rune &result, Cursor<Unit> &in) {
        Unit first = in.next();

        if (len(first) > in.rem()) {
            result = U'�';
            return false;
        }

        if (first >= 0xd800 && first <= 0xdbff) {
            if (in.rem() < 2) {
                return false;
            }

            Unit second = in.next();

            if (second < 0xdc00 || second > 0xdfff) {
                return false;
            }

            result = ((first - 0xd800) << 10) | (second - 0xdc00) + 0x10000;
        } else {
            result = first;
        }

        return true;
    }

    static bool encode(Rune c, MutCursor<Unit> &out) {
        if (c <= 0xffff) {
            out.put(c);
            return true;
        } else if (c <= 0x10ffff) {
            out.put(0xd800 | ((c - 0x10000) >> 10));
            out.put(0xdc00 | ((c - 0x10000) & 0x3ff));
            return true;
        } else {
            return encode(U'�', out);
        }
    }
};

[[gnu::used]] static inline Utf16 UTF16;

static_assert(Encoding<Utf16>);

/* --- Utf32 ---------------------------------------------------------------- */

struct Utf32 {
    using Unit = char32_t;

    static constexpr size_t len(Unit) {
        return 1;
    }

    static bool decode(Rune &result, Cursor<Unit> &in) {
        result = in.next();
        return true;
    }

    static bool encode(Rune c, MutCursor<Unit> &out) {
        out.put(c);
        return false;
    }
};

[[gnu::used]] static inline Utf32 UTF32;

static_assert(Encoding<Utf32>);

/* --- Ascii ---------------------------------------------------------------- */

struct Ascii {
    using Unit = char;

    static constexpr size_t len(Unit) {
        return 1;
    }

    static bool decode(Rune &result, Cursor<Unit> &in) {
        auto c = in.next();
        if (c >= 0) {
            result = c;
            return true;
        } else {
            result = U'�';
            return false;
        }
    }

    static bool encode(Rune c, MutCursor<Unit> &out) {
        if (c < 0) {
            out.put('?');
            return false;
        }

        out.put(c);
        return true;
    }
};

[[gnu::used]] static inline Ascii ASCII;

static_assert(Encoding<Ascii>);

/* --- Extended Ascii ------------------------------------------------------- */

template <typename Mapper>
struct EAscii {
    using Unit = uint8_t;

    static constexpr size_t len(Unit) {
        return 1;
    }

    static bool decode(Rune &result, Cursor<Unit> &in) {
        Mapper mapper;
        result = mapper(in.next());
        return true;
    }

    static bool encode(Rune c, MutCursor<Unit> &out) {
        Mapper mapper;
        for (Unit i = 0; i <= 255; i++) {
            if (mapper(i) == c) {
                out.put(i);
                return true;
            }
        }

        out.put('?');
        return false;
    }
};

/* --- Ibm437 --------------------------------------------------------------- */

// clang-format off
using Ibm437Mapper = decltype([](uint8_t c) {
    return Array<Rune, 256> {
        U'\0', U'☺', U'☻', U'♥', U'♦', U'♣', U'♠', U'•',
        U'◘', U'○', U'◙', U'♂', U'♀', U'♪', U'♫', U'☼',
        U'►', U'◄', U'↕', U'‼', U'¶', U'§', U'▬', U'↨',
        U'↑', U'↓', U'→', U'←', U'∟', U'↔', U'▲', U'▼',
        U' ', U'!', U'"', U'#', U'$', U'%', U'&', U'\'',
        U'(', U')', U'*', U'+', U',', U'-', U'.', U'/',
        U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7',
        U'8', U'9', U':', U';', U'<', U'=', U'>', U'?',
        U'@', U'A', U'B', U'C', U'D', U'E', U'F', U'G',
        U'H', U'I', U'J', U'K', U'L', U'M', U'N', U'O',
        U'P', U'Q', U'R', U'S', U'T', U'U', U'V', U'W',
        U'X', U'Y', U'Z', U'[', U'\\', U']', U'^', U'_',
        U'`', U'a', U'b', U'c', U'd', U'e', U'f', U'g',
        U'h', U'i', U'j', U'k', U'l', U'm', U'n', U'o',
        U'p', U'q', U'r', U's', U't', U'u', U'v', U'w',
        U'x', U'y', U'z', U'{', U'|', U'}', U'~',  U'⌂',
        U'Ç', U'ü', U'é', U'â', U'ä', U'à', U'å', U'ç',
        U'ê', U'ë', U'è', U'ï', U'î', U'ì', U'Ä', U'Å',
        U'É', U'æ', U'Æ', U'ô', U'ö', U'ò', U'û', U'ù',
        U'ÿ', U'Ö', U'Ü', U'¢', U'£', U'¥', U'₧', U'ƒ',
        U'á', U'í', U'ó', U'ú', U'ñ', U'Ñ', U'ª', U'º',
        U'¿', U'⌐', U'¬', U'½', U'¼', U'¡', U'«', U'»',
        U'░', U'▒', U'▓', U'│', U'┤', U'╡', U'╢', U'╖',
        U'╕', U'╣', U'║', U'╗', U'╝', U'╜', U'╛', U'┐',
        U'└', U'┴', U'┬', U'├', U'─', U'┼', U'╞', U'╟',
        U'╚', U'╔', U'╩', U'╦', U'╠', U'═', U'╬', U'╧',
        U'╨', U'╤', U'╥', U'╙', U'╘', U'╒', U'╓', U'╫',
        U'╪', U'┘', U'┌', U'█', U'▄', U'▌', U'▐', U'▀',
        U'α', U'ß', U'Γ', U'π', U'Σ', U'σ', U'µ', U'τ',
        U'Φ', U'Θ', U'Ω', U'δ', U'∞', U'φ', U'ε', U'∩',
        U'≡', U'±', U'≥', U'≤', U'⌠', U'⌡', U'÷', U'≈',
        U'°', U'∙', U'·', U'√', U'ⁿ', U'²', U'■', U'\x00a0',
    }[c];
});
// clang-format on

using Ibm437 = EAscii<Ibm437Mapper>;

[[gnu::used]] static inline Ibm437 IBM437;

static_assert(Encoding<Ibm437>);

/* --- Latin1 --------------------------------------------------------------- */

using Latin1Mapper = decltype([](uint8_t c) {
    // FIXME: """"Unicode is a "superset" of Latin1""" (please note the quotes)
    return (Rune)c;
});

using Latin1 = EAscii<Latin1Mapper>;

[[gnu::used]] static inline Latin1 LATIN1;

static_assert(Encoding<Latin1>);

/* --- Utilities ------------------------------------------------------------ */

template <Encoding Source, Encoding Target>
size_t transcode_len(Cursor<typename Source::Unit> input) {
    size_t result = 0;

    while (input.rem()) {
        Rune r;
        if (Source::decode(r, input)) {
            result += Target::len(r);
        } else {
            result += Target::len(U'�');
        }
    }

    return result;
}

template <Encoding Source>
size_t transcode_len(Cursor<typename Source::Unit> input) {
    size_t result = 0;

    while (input.rem()) {
        Rune r;
        if (Source::decode(r, input)) {
            result += 1;
        }
    }

    return result;
}

template <Encoding Source, Encoding Target>
size_t transcode_units(Cursor<typename Source::Unit> input, MutCursor<typename Target::Unit> output) {
    size_t result = 0;

    while (input.rem()) {
        Rune r;
        if (Source::decode(r, input)) {
            result += Target::encode(r, output);
        } else {
            result += Target::encode(U'�', output);
        }
    }

    return result;
}

} // namespace Karm
