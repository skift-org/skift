#pragma once

#include <karm-base/string.h>

#include "scan.h"

namespace Karm::Re {

inline auto single(auto... c) {
    return [=](Text::Scan &scan) {
        if (((scan.peek() == (Rune)c) || ...)) {
            scan.next();
            return true;
        } else {
            return false;
        }
    };
}

inline auto word(Str word) {
    return [=](Text::Scan &scan) {
        return scan.skip(word);
    };
}

inline auto range(Rune start, Rune end) {
    return [=](Text::Scan &scan) {
        return scan.peek() >= start && scan.peek() <= end;
    };
}

template <typename... Exprs>
inline auto either(Exprs... exprs) {
    return [=](Text::Scan &scan) {
        return (exprs(scan) || ...);
    };
}

template <typename... Exprs>
inline auto chain(Exprs... exprs) {
    return [=](Text::Scan &scan) {
        return (exprs(scan) && ...);
    };
}

inline auto negate(auto expr) {
    return [=](Text::Scan &scan) {
        return !expr(scan);
    };
}

/* --- Quantifiers ---------------------------------------------------------- */

inline auto zero_or_more(auto expr) {
    return [=](Text::Scan &scan) {
        while (expr(scan))
            ;
        return true;
    };
}

inline auto one_or_more(auto expr) {
    return [=](Text::Scan &scan) {
        if (!expr(scan))
            return false;
        while (expr(scan))
            ;
        return true;
    };
}

inline auto zero_or_one(auto expr) {
    return [=](Text::Scan &scan) {
        expr(scan);
        return true;
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
        zero_or_more(space()),
        single(r),
        zero_or_more(space()));
}

inline auto separator(Str w) {
    return chain(
        zero_or_more(space()),
        word(w),
        zero_or_more(space()));
}

} // namespace Karm::Re
