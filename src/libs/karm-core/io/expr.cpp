export module Karm.Core:io.expr;

import :io.sscan;

namespace Karm::Re {

/// Match a string against an Expr
///
/// Returns
///  - Match::YES : If the Expr match the whole string
///  - Match::NO : If the Expr doesn't match the string
///  - Match::PARTIAL : If the Expr matches but the end of string is not reached
export template <StaticEncoding E>
Match match(Expr auto expr, _Str<E> input) {
    Io::_SScan<E> scan(input);
    if (not expr(scan))
        return Match::NO;
    return scan.ended()
               ? Match::YES
               : Match::PARTIAL;
}

// MARK: Combinators -----------------------------------------------------------

/// Returns true if either of the expressions match pass as parameters
export constexpr Expr auto either(Expr auto... exprs) {
    return [=](auto& scan) {
        return (exprs(scan) or ...);
    };
}

export constexpr Expr auto operator|(Expr auto a, Expr auto b) {
    return either(a, b);
}

/// Returns true if all of the expressions match.
/// The expressions are evaluated in order.
/// If any expression fails, the scanner is rewound to the state before the first expression.
export constexpr Expr auto chain(Expr auto... exprs) {
    return [=](auto& scan) {
        auto saved = scan;
        if ((exprs(scan) and ...))
            return true;
        scan = saved;
        return false;
    };
}

export constexpr Expr auto operator&(Expr auto a, Expr auto b) {
    return chain(a, b);
}

/// Inverts the result of the expression.
export constexpr Expr auto negate(Expr auto expr) {
    return [=](auto& scan) {
        auto saved = scan;
        if (not expr(scan)) {
            if (scan.ended())
                return false;
            scan.next();
            return true;
        }
        scan = saved;
        return false;
    };
}

export constexpr Expr auto operator~(Expr auto expr) {
    return negate(expr);
}

/// Consumes until the expression matches or the end of the input is reached.
/// scanner is rewound to the last unmatched rune.
export constexpr Expr auto until(Expr auto expr) {
    return [=](auto& scan) {
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
export constexpr Expr auto untilAndConsume(Expr auto expr) {
    return [=](auto& scan) {
        while (not expr(scan) and not scan.ended())
            scan.next();
        return true;
    };
}

// MARK: Quantifiers -----------------------------------------------------------

/// Try to match an expression `atLeast` times and and stops when `atMost` times
/// is reached, return true if atLeast is reached otherwise rewind the scanner.
export constexpr Expr auto nOrN(usize atLeast, usize atMost, Expr auto expr) {
    return [=](auto& scan) {
        usize count = 0;
        auto saved = scan;
        saved = scan;
        while (expr(scan)) {
            count++;
            saved = scan;
            if (count == atMost)
                break;
        }
        if (count >= atLeast) {
            scan = saved;
            return true;
        }
        return false;
    };
}

/// Returns true if the expression matches exactly n times.
export constexpr Expr auto exactly(usize n, Expr auto expr) {
    return nOrN(n, n, expr);
}

/// Returns true if the expression matches at least n times.
export constexpr Expr auto atLeast(usize n, Expr auto expr) {
    return nOrN(n, Limits<usize>::MAX, expr);
}

/// Returns true if the expression matches at most n times.
export constexpr Expr auto atMost(usize n, Expr auto expr) {
    return nOrN(0, n, expr);
}

/// Returns true if the expression matches zero or more times.
export constexpr Expr auto zeroOrMore(Expr auto expr) {
    return nOrN(0, Limits<usize>::MAX, expr);
}

/// Returns true if the expression matches one or more times.
export constexpr Expr auto oneOrMore(Expr auto expr) {
    return nOrN(1, Limits<usize>::MAX, expr);
}

/// Returns true if the expression matches zero or one times.
export constexpr Expr auto zeroOrOne(Expr auto expr) {
    return nOrN(0, 1, expr);
}

// Returns true if the expression matches at most n times.
// If the expression matches more than n times, the scanner is rewound to the start of the first match.
export constexpr Expr auto atMost(usize n, auto expr) {
    return nOrN(0, n, expr);
}

/// Returns true if the expression matches exactly one time and saves the result.
export Expr auto token(Str& out, Expr auto expr) {
    return [=, &out](auto& scan) {
        scan.begin();
        if (expr(scan)) {
            out = scan.end();
            return true;
        }
        return false;
    };
}

/// If the expression matches, the callback is called with the matching scanner.
/// The scanner is rewound to the state before the expression was matched.
export Expr auto trap(Expr auto expr, auto cb) {
    return [=](auto& scan) {
        auto saved = scan;
        if (expr(scan)) {
            scan = saved;
            cb(saved);
            return false;
        }
        return true;
    };
}

// MARK: Tokens ----------------------------------------------------------------

/// Match nothing and return true.
export constexpr Expr auto nothing() {
    return [](auto&) {
        return true;
    };
}

export constexpr Expr auto any() {
    return [](auto& scan) {
        scan.next();
        return true;
    };
}

/// Match the end of the input.
export constexpr Expr auto eof() {
    return [](auto& scan) {
        return scan.ended();
    };
}

/// Match a word and consume it.
export constexpr Expr auto word(Str word) {
    return [=](auto& scan) {
        return scan.skip(word);
    };
}

/// Match a single character and consume it.
/// Multiple characters can be passed to match any of them.
export constexpr Expr auto single(auto... c) {
    return [=](auto& scan) {
        if (((scan.peek() == (Rune)c) or ...)) {
            scan.next();
            return true;
        }
        return false;
    };
}

/// Match a single character against a ctype function and consume it.
export constexpr Expr auto ctype(CType auto ctype) {
    return [=](auto& scan) {
        if (ctype(scan.peek())) {
            scan.next();
            return true;
        }
        return false;
    };
}

/// Match a character range and consume it if it lies within the range.
export constexpr Expr auto range(Rune start, Rune end) {
    return [=](auto& scan) {
        if (scan.peek() >= start and scan.peek() <= end) {
            scan.next();
            return true;
        }
        return false;
    };
}

// MARK: Posix Classes ---------------------------------------------------------

/// Match an ASCII Character and consume it.
export constexpr Expr auto ascii() {
    return ctype(isAscii);
}

/// Match an ASCII upper case letter and consume it.
export constexpr Expr auto upper() {
    return ctype(isAsciiUpper);
}

/// Match an ASCII lower case letter and consume it.
export constexpr Expr auto lower() {
    return ctype(isAsciiLower);
}

/// Match an ASCII letter and consume it.
export constexpr Expr auto alpha() {
    return upper() | lower();
}

/// Match an ASCII digit and consume it.
export constexpr Expr auto digit() {
    return ctype(isAsciiDecDigit);
}

/// Match an ASCII hexadecimal digit and consume it.
export constexpr Expr auto xdigit() {
    return ctype(isAsciiHexDigit);
}

/// Match an ASCII alphanumeric character and consume it.
export constexpr Expr auto alnum() {
    return ctype(isAsciiAlphaNum);
}

/// Match a word made of ASCII letters and underscores and consume it.
export constexpr Expr auto word() {
    return alnum() | single('_');
}

/// Match punctuation and consume it.
export constexpr Expr auto punct() {
    return ctype(isAsciiPunct);
}

/// Match ascii whitespace and consume it.
export constexpr Expr auto space() {
    return ctype(isAsciiSpace);
}

/// Match a blank space and consume it.
export constexpr Expr auto blank() {
    return ctype(isAsciiBlank);
}

// MARK: Utils -----------------------------------------------------------------

/// Match a separator and consume it.
/// A separator is a expr surrounded by spaces.
export constexpr Expr auto separator(Expr auto expr) {
    return zeroOrMore(space()) &
           expr &
           zeroOrMore(space());
}

/// Match an optional separator and consume it.
/// A separator is an expression surrounded by spaces.
/// If the separator is not found, the expression still matches.
/// And whitespaces are consumed.
export constexpr Expr auto optSeparator(Expr auto expr) {
    return zeroOrMore(space()) &
           zeroOrOne(expr) &
           zeroOrMore(space());
}

} // namespace Karm::Re

export constexpr Karm::Re::Expr auto operator""_re(char const* str, Karm::usize len) {
    return Karm::Re::word(Karm::Str{str, len});
}

export constexpr Karm::Re::Expr auto operator""_re(char c) {
    return Karm::Re::single(c);
}
