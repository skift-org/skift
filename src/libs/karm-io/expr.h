#pragma once

#include <karm-base/string.h>

#include "sscan.h"

namespace Karm::Re {

template <typename T>
concept Expr = requires(T expr, Io::SScan &scan) {
    { expr(scan) } -> Meta::Same<bool>;
};

bool match(Re::Expr auto expr, Str input) {
    Io::SScan scan(input);
    return expr(scan);
}

/* --- Combinators ---------------------------------------------------------- */

/// Returns true if any of the expressions match.
inline auto either(Expr auto... exprs) {
    return [=](auto &scan) {
        return (exprs(scan) or ...);
    };
}

/// Returns true if all of the expressions match.
/// The expressions are evaluated in order.
/// If any expression fails, the scanner is rewound to the state before the first expression.
inline auto chain(Expr auto... exprs) {
    return [=](auto &scan) {
        auto saved = scan;
        if ((exprs(scan) and ...)) {
            return true;
        }

        scan = saved;
        return false;
    };
}

/// Inverts the result of the expression.
inline auto negate(Expr auto expr) {
    return [=](auto &scan) {
        return not expr(scan);
    };
}

/// Consumes until the expression matches or the end of the input is reached.
/// scanner is rewound to the last unmatched rune.
inline auto until(Expr auto expr) {
    return [=](auto &scan) {
        auto saved = scan;
        while (not expr(scan) and not scan.ended()) {
            scan.next();
            saved = scan;
        }
        scan = saved;
        return true;
    };
}

/// Consumes until the expression matches or the end of the input is reached.
inline auto untilAndConsume(Expr auto expr) {
    return [=](auto &scan) {
        while (not expr(scan) and not scan.ended()) {
            scan.next();
        }
        return true;
    };
}

/* --- Quantifiers ---------------------------------------------------------- */

/// Returns true if the expression matches zero or more times.
inline auto zeroOrMore(Expr auto expr) {
    return [=](auto &scan) {
        while (expr(scan))
            ;
        return true;
    };
}

/// Returns true if the expression matches one or more times.
inline auto oneOrMore(Expr auto expr) {
    return [=](auto &scan) {
        auto saved = scan;
        if (not expr(scan)) {
            scan = saved;
            return false;
        }
        return zeroOrMore(expr)(scan);
    };
}

/// Returns true if the expression matches zero or one times.
inline auto zeroOrOne(Expr auto expr) {
    return [=](auto &scan) {
        expr(scan);
        return true;
    };
}

/// Returns true if the expression matches exactly one time and saves the result.
inline auto token(Str &out, Expr auto expr) {
    return [=, &out](auto &scan) {
        scan.begin();
        if (expr(scan)) {
            out = scan.end();
            return true;
        }
        return false;
    };
}

/* --- Tokens --------------------------------------------------------------- */

/// Math nothing and return true.
inline auto nothing() {
    return [](auto &) {
        return true;
    };
}

/// Match the end of the input.
inline auto eof() {
    return [](auto &scan) {
        return scan.ended();
    };
}

/// Match a word and consume it.
inline auto word(Str word) {
    return [=](auto &scan) {
        return scan.skip(word);
    };
}

/// Match a single character and consume it.
/// Multiple characters can be passed to match any of them.
inline auto single(auto... c) {
    return [=](auto &scan) {
        if (((scan.curr() == (Rune)c) or ...)) {
            scan.next();
            return true;
        }
        return false;
    };
}

/// Match a character range and consume it if it lies within the range.
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

/// Match an ASCII upper case letter and consume it.
inline auto upper() {
    return range('A', 'Z');
}

/// Match an ASCII lower case letter and consume it.
inline auto lower() {
    return range('a', 'z');
}

/// Match an ASCII letter and consume it.
inline auto alpha() {
    return either(upper(), lower());
}

/// Match an ASCII digit and consume it.
inline auto digit() {
    return range('0', '9');
}

/// Match an ASCII hexadecimal digit and consume it.
inline auto xdigit() {
    return either(digit(), range('a', 'f'), range('A', 'F'));
}

/// Match an ASCII alphanumeric character and consume it.
inline auto alnum() {
    return either(alpha(), digit());
}

/// Match a work made of ASCII letters and underscores and consume it.
inline auto word() {
    return either(alnum(), single('_'));
}

/// Match punctuation and consume it.
inline auto punct() {
    return single(
        '!', '"', '#', '$', '%', '&', '\'', '(',
        ')', '*', '+', ',', '-', '.', '/', ':',
        ';', '<', '=', '>', '?', '@', '[', '\\',
        ']', '^', '_', '`', '{', '|', '}', '~'
    );
}

/// Match ascii whitespace and consume it.
inline auto space() {
    return single(' ', '\t', '\n', '\r');
}

/// Match a blank space and consume it.
inline auto blank() {
    return single(' ', '\t');
}

/* --- Utils ---------------------------------------------------------------- */

/// Match a separator and consume it.
/// A separator is a rune surrounded by spaces.
inline auto separator(Rune r) {
    return chain(
        zeroOrMore(space()),
        single(r),
        zeroOrMore(space())
    );
}

/// Match a separator and consume it.
/// A separator is a word surrounded by spaces.
inline auto separator(Str w) {
    return chain(
        zeroOrMore(space()),
        word(w),
        zeroOrMore(space())
    );
}

/// Match an optional separator and consume it.
/// A separator is a rune surrounded by spaces.
/// If the separator is not found, the expression still matches.
/// And whitespaces are consumed.
inline auto optSeparator(Rune r) {
    return chain(
        zeroOrMore(space()),
        zeroOrOne(single(r)),
        zeroOrMore(space())
    );
}

/// Match an optional separator and consume it.
/// A separator is a word surrounded by spaces.
/// If the separator is not found, the expression still matches.
/// And whitespaces are consumed.
inline auto optSeparator(Str w) {
    return chain(
        zeroOrMore(space()),
        zeroOrOne(word(w)),
        zeroOrMore(space())
    );
}

} // namespace Karm::Re
