#pragma once

#include <karm-base/string.h>

namespace Karm::Re {

inline auto either(auto... exprs) {
    return [=](auto &scan) {
        return (exprs(scan) or ...);
    };
}

inline auto chain(auto... exprs) {
    return [=](auto &scan) {
        auto saved = scan;
        if ((exprs(scan) and ...)) {
            return true;
        }

        scan = saved;
        return false;
    };
}

inline auto negate(auto expr) {
    return [=](auto &scan) {
        return not expr(scan);
    };
}

inline auto until(auto expr) {
    return [=](auto &scan) {
        while (not expr(scan) and not scan.ended())
            scan.next();
        return true;
    };
}

/* --- Quantifiers ---------------------------------------------------------- */

inline auto zeroOrMore(auto expr) {
    return [=](auto &scan) {
        while (expr(scan))
            ;
        return true;
    };
}

inline auto oneOrMore(auto expr) {
    return [=](auto &scan) {
        auto saved = scan;
        if (not expr(scan)) {
            scan = saved;
            return false;
        }
        return zeroOrMore(expr)(scan);
    };
}

inline auto zeroOrOne(auto expr) {
    return [=](auto &scan) {
        expr(scan);
        return true;
    };
}

/* --- Tokens --------------------------------------------------------------- */

inline auto nil() {
    return [](auto &) {
        return false;
    };
}

inline auto any() {
    return [](auto &) {
        return true;
    };
}

inline auto eof() {
    return [](auto &scan) {
        return scan.ended();
    };
}

inline auto word(Str word) {
    return [=](auto &scan) {
        return scan.skip(word);
    };
}

inline auto single(auto... c) {
    return [=](auto &scan) {
        if (((scan.curr() == (Rune)c) or ...)) {
            scan.next();
            return true;
        }
        return false;
    };
}

inline auto range(Rune start, Rune end) {
    return [=](auto &scan) {
        if (scan.curr() >= start and scan.curr() <= end) {
            scan.next();
            return true;
        }
        return false;
    };
}

/* --- Posix Classes -------------------------------------------------------- */

inline auto upper() {
    return range('A', 'Z');
}

inline auto lower() {
    return range('a', 'z');
}

inline auto alpha() {
    return either(upper(), lower());
}

inline auto digit() {
    return range('0', '9');
}

inline auto xdigit() {
    return either(digit(), range('a', 'f'), range('A', 'F'));
}

inline auto alnum() {
    return either(alpha(), digit());
}

inline auto word() {
    return either(alnum(), single('_'));
}

inline auto punct() {
    return single(
        '!', '"', '#', '$', '%', '&', '\'', '(',
        ')', '*', '+', ',', '-', '.', '/', ':',
        ';', '<', '=', '>', '?', '@', '[', '\\',
        ']', '^', '_', '`', '{', '|', '}', '~');
}

inline auto space() {
    return single(' ', '\t', '\n', '\r');
}

inline auto blank() {
    return single(' ', '\t');
}

/* --- Utils ---------------------------------------------------------------- */

inline auto separator(Rune r) {
    return chain(
        zeroOrMore(space()),
        single(r),
        zeroOrMore(space()));
}

inline auto separator(Str w) {
    return chain(
        zeroOrMore(space()),
        word(w),
        zeroOrMore(space()));
}

inline auto optSeparator(Rune r) {
    return chain(
        zeroOrMore(space()),
        zeroOrOne(single(r)),
        zeroOrMore(space()));
}

inline auto optSeparator(Str w) {
    return chain(
        zeroOrMore(space()),
        zeroOrOne(word(w)),
        zeroOrMore(space()));
}

} // namespace Karm::Re
