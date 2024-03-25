#pragma once

#include <karm-base/ctype.h>
#include <karm-base/string.h>

#include "sscan.h"

namespace Karm::Re {

template <typename T>
concept Expr = requires(T expr, Io::SScan &scan) {
    { expr(scan) } -> Meta::Same<bool>;
};

Match match(Re::Expr auto expr, Str input) {
    Io::SScan scan(input);
    if (not expr(scan))
        return Match::NO;
    return scan.ended() ? Match::YES : Match::PARTIAL;
}

/* --- Combinators ---------------------------------------------------------- */

/// Returns true if any of the expressions match.
inline auto either(Expr auto... exprs) {
    return [=](auto &scan) {
        return (exprs(scan) or ...);
    };
}

auto operator|(Expr auto a, Expr auto b) {
    return either(a, b);
}

/// Returns true if all of the expressions match.
/// The expressions are evaluated in order.
/// If any expression fails, the scanner is rewound to the state before the first expression.
inline auto chain(Expr auto... exprs) {
    return [=](auto &scan) {
        auto saved = scan;
        if ((exprs(scan) and ...))
            return true;
        scan = saved;
        return false;
    };
}

auto operator&(Expr auto a, Expr auto b) {
    return chain(a, b);
}

/// Inverts the result of the expression.
inline auto negate(Expr auto expr) {
    return [=](auto &scan) {
        return not expr(scan);
    };
}

auto operator~(Expr auto expr) {
    return negate(expr);
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
        while (not expr(scan) and not scan.ended())
            scan.next();
        return true;
    };
}

/* --- Quantifiers ---------------------------------------------------------- */

/// Returns true if the expression matches exactly n times.
inline auto exactly(usize n, Expr auto expr) {
    return [=](auto &scan) {
        auto saved = scan;
        for (usize i = 0; i < n; ++i) {
            if (not expr(scan)) {
                scan = saved;
                return false;
            }
        }
        return true;
    };
}

/// Returns true if the expression matches at least n times.
inline auto atLeast(usize n, Expr auto expr) {
    return [=](auto &scan) {
        auto saved = scan;
        for (usize i = 0; i < n; ++i) {
            if (not expr(scan)) {
                scan = saved;
                return false;
            }
        }
        return zeroOrMore(expr)(scan);
    };
}

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

inline auto atMost(usize n, auto expr) {
    return [=](auto &scan) {
        for (usize i = 0; i < n; ++i) {
            if (not expr(scan)) {
                return true;
            }
        }
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

inline auto trap(auto expr, auto cb) {
    return [=](auto &scan) {
        auto saved = scan;
        if (expr(scan)) {
            scan = saved;
            cb(saved);
            return false;
        }
        return true;
    };
}

/* --- Tokens --------------------------------------------------------------- */

/// Match nothing and return true.
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

/// Match a single character against a ctype function and consume it.
inline auto ctype(CType auto ctype) {
    return [=](auto &scan) {
        if (ctype(scan.curr())) {
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

/// Match an ASCII Character and consume it.
inline auto ascii() {
    return ctype(isAscii);
}

/// Match an ASCII upper case letter and consume it.
inline auto upper() {
    return ctype(isAsciiUpper);
}

/// Match an ASCII lower case letter and consume it.
inline auto lower() {
    return ctype(isAsciiLower);
}

/// Match an ASCII letter and consume it.
inline auto alpha() {
    return upper() | lower();
}

/// Match an ASCII digit and consume it.
inline auto digit() {
    return ctype(isDecDigit);
}

/// Match an ASCII hexadecimal digit and consume it.
inline auto xdigit() {
    return ctype(isHexDigit);
}

/// Match an ASCII alphanumeric character and consume it.
inline auto alnum() {
    return ctype(isAsciiAlphaNum);
}

/// Match a work made of ASCII letters and underscores and consume it.
inline auto word() {
    return alnum() | single('_');
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
/// A separator is a expr surrounded by spaces.
inline auto separator(Expr auto expr) {
    return zeroOrMore(space()) &
           expr &
           zeroOrMore(space());
}

/// Match an optional separator and consume it.
/// A separator is an expression surrounded by spaces.
/// If the separator is not found, the expression still matches.
/// And whitespaces are consumed.
inline auto optSeparator(Expr auto expr) {
    return zeroOrMore(space()) &
           zeroOrOne(expr) &
           zeroOrMore(space());
}

} // namespace Karm::Re

inline auto operator""_re(char const *str, usize len) {
    return Karm::Re::word(Str{str, len});
}

inline auto operator""_re(char c) {
    return Karm::Re::single(c);
}
