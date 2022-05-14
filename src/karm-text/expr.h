#pragma once

#include <karm-base/string.h>

#include "scan.h"

namespace Re {

inline auto single(auto... c) {
    return [=](Karm::Text::Scan &scan) {
        return ((scan.peek() == (Karm::Base::Rune)c) || ...);
    };
}

inline auto word(Karm::Base::Str word) {
    return [=](Karm::Text::Scan &scan) {
        return scan.skip(word);
    };
}

inline auto range(Karm::Base::Rune start, Karm::Base::Rune end) {
    return [=](Karm::Text::Scan &scan) {
        return scan.peek() >= start && scan.peek() <= end;
    };
}

template <typename... Exprs>
inline auto range_or(Exprs... exprs) {
    return [=](Karm::Text::Scan &scan) {
        return (exprs(scan) || ...);
    };
}

template <typename... Exprs>
inline auto range_and(Exprs... exprs) {
    return [=](Karm::Text::Scan &scan) {
        return (exprs(scan) && ...);
    };
}

/* --- Quantifiers ---------------------------------------------------------- */

inline auto zero_or_more(auto expr) {
    return [=](Karm::Text::Scan &scan) {
        while (expr(scan))
            ;
        return true;
    };
}

inline auto one_or_more(auto expr) {
    return [=](Karm::Text::Scan &scan) {
        if (!expr(scan))
            return false;
        while (expr(scan))
            ;
        return true;
    };
}

inline auto zero_or_one(auto expr) {
    return [=](Karm::Text::Scan &scan) {
        expr(scan);
        return true;
    };
}

/* --- Posix Classes -------------------------------------------------------- */

inline auto
posix_upper() {
    return range('A', 'Z');
}

inline auto posix_lower() {
    return range('a', 'z');
}

inline auto posix_alpha() {
    return range_or(posix_upper(), posix_lower());
}

inline auto posix_digit() {
    return range('0', '9');
}

inline auto posix_xdigit() {
    return range_or(posix_digit(), range('a', 'f'), range('A', 'F'));
}

inline auto posix_alnum() {
    return range_or(posix_alpha(), posix_digit());
}

inline auto posix_punct() {
    return single(
        '!', '"', '#', '$', '%', '&', '\'', '(',
        ')', '*', '+', ',', '-', '.', '/', ':',
        ';', '<', '=', '>', '?', '@', '[', '\\',
        ']', '^', '_', '`', '{', '|', '}', '~');
}

inline auto posix_space() {
    return single(' ', '\t', '\n', '\r');
}

/* --- Utils ---------------------------------------------------------------- */

inline auto separator(Karm::Base::Rune rune) {
    return range_and(
        zero_or_more(posix_space()),
        single(rune),
        zero_or_more(posix_space()));
}

inline auto separator(Karm::Base::Str word) {
    return range_and(
        zero_or_more(posix_space()),
        Re::word(word),
        zero_or_more(posix_space()));
}

} // namespace Re
