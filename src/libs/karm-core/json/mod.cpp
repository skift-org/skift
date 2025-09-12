module;

#include <karm-core/macros.h>

export module Karm.Core:json;

import :serde;
import :io.aton;
import :io.emit;
import :io.expr;
import :io.funcs;

namespace Karm::Json {

// MARK: Parse -----------------------------------------------------------------

export Res<Serde::Value> parse(Io::SScan& s);

Res<String> parseStr(Io::SScan& s) {
    if (not s.skip('"'))
        return Error::invalidData("expected '\"'");

    s.begin();

    while (not s.ended()) {
        if (s.peek() == '"') {
            auto str = s.end();
            s.next();
            return Ok(String{str});
        }

        if (s.skip('\\')) {
            if (s.skip('"')) {
                continue;
            }
            if (s.skip('\\')) {
                continue;
            }
            if (s.skip('/')) {
                continue;
            }
            if (s.skip('b')) {
                continue;
            }
            if (s.skip('f')) {
                continue;
            }
            if (s.skip('n')) {
                continue;
            }
            if (s.skip('r')) {
                continue;
            }
            if (s.skip('t')) {
                continue;
            }
            if (s.skip('u')) {
                if (Io::atou(s, {.base = 16})) {
                    continue;
                }
            }
        }

        if (s.next())
            continue;

        return Error::invalidData("invalid string");
    }

    return Error::invalidData("expected '\"'");
}

Res<Serde::Object> parseObject(Io::SScan& s) {
    Serde::Object m;
    if (not s.skip('{')) {
        return Error::invalidData("expected '{'");
    }

    if (s.skip('}')) {
        return Ok(m);
    }

    while (true) {
        s.eat(Re::space());
        auto key = try$(parseStr(s));

        s.eat(Re::space());
        if (not s.skip(':'))
            return Error::invalidData("expected ':'");

        s.eat(Re::space());

        auto value = try$(parse(s));
        m.put(key, value);

        s.eat(Re::space());

        if (s.skip('}'))
            return Ok(m);

        if (not s.skip(','))
            return Error::invalidData("expected ','");
    }
}

Res<Serde::Array> parseArray(Io::SScan& s) {
    Serde::Array v;
    if (not s.skip('['))
        return Error::invalidData("expected '['");

    if (s.skip(']'))
        return Ok(v);

    while (true) {
        s.eat(Re::space());

        auto value = try$(parse(s));
        v.pushBack(value);

        s.eat(Re::space());

        if (s.skip(']'))
            return Ok(v);

        if (not s.skip(','))
            return Error::invalidData("expected ','");
    }
}

static auto const RE_NUMBER_START = '-'_re | Re::digit();

Res<usize> parseDigits(Io::SScan& s) {
    if (s.ended())
        return Error::invalidData("unexpected end of input");

    if (s.match(Re::digit()) == Match::NO)
        return Error::invalidData("expected digit");

    usize digits = 0;
    while (s.match(Re::digit()) != Match::NO) {
        s.next();
        ++digits;
    }

    return Ok(digits);
}

#ifndef __ck_freestanding__
Res<f64> parseDecimal(Io::SScan& s) {
    if (s.ended())
        return Error::invalidData("unexpected end of input");

    if (s.match(Re::digit()) == Match::NO)
        return Error::invalidData("expected digit");

    f64 fpart = 0;
    f64 multiplier = 0.1;
    while (s.match(Re::digit()) != Match::NO) {
        fpart += parseAsciiDecDigit(s.next()) * multiplier;
        multiplier /= 10;
    }

    return Ok(fpart);
}
#endif

Res<isize> parseInteger(Io::SScan& s) {
    bool sign = false;

    if (s.skip('-'))
        sign = true;
    else
        s.skip('+');

    if (s.skip('0'))
        return Ok(0);

    if (s.ended())
        return Error::invalidData("unexpected end of input");

    isize ipart = 0;
    ipart = parseAsciiDecDigit(s.next());
    while (s.match(Re::digit()) != Match::NO) {
        ipart = ipart * 10 + parseAsciiDecDigit(s.next());
    }

    return Ok(sign ? -ipart : ipart);
}

Res<Serde::Value> parseNumber(Io::SScan& s) {
    isize ipart = try$(parseInteger(s));

    if (s.match(Re::single('.', 'e', 'E')) == Match::NO)
        return Ok<Serde::Value>(ipart);

// NOTE: Floating point numbers are not supported in freestanding environments.
#ifdef __ck_freestanding__
    return Error::invalidData("floating point numbers are not supported");
#else
    f64 fpart = 0.0;
    if (s.skip('.')) {
        fpart = try$(parseDecimal(s));
    }

    if (s.skip(Re::single('e', 'E'))) {
        bool expSign = false;
        if (s.skip('-'))
            expSign = true;
        else
            s.skip('+');

        isize exp = try$(parseDigits(s));

        if (expSign)
            exp = -exp;

        return Ok<Serde::Value>(ipart + fpart * Math::pow<isize>(10, exp));
    }

    return Ok<Serde::Value>(ipart + fpart);
#endif
}

export Res<Serde::Value> parse(Io::SScan& s) {
    s.eat(Re::space());

    if (s.ended()) {
        return Error::invalidData("unexpected end of input");
    } else if (s.peek() == '{') {
        return Ok(Serde::Value{try$(parseObject(s))});
    } else if (s.peek() == '[') {
        return Ok(Serde::Value{try$(parseArray(s))});
    } else if (s.peek() == '"') {
        return Ok(Serde::Value{try$(parseStr(s))});
    } else if (s.skip("null")) {
        return Ok(Serde::Value{NONE});
    } else if (s.skip("true")) {
        return Ok(Serde::Value{true});
    } else if (s.skip("false")) {
        return Ok(Serde::Value{false});
    } else if (s.match(RE_NUMBER_START) != Match::NO) {
        return parseNumber(s);
    }

    return Error::invalidData("unexpected character");
}

export Res<Serde::Value> parse(Str s) {
    Io::SScan scan{s};
    return parse(scan);
}

// MARK: Unparse ---------------------------------------------------------------

export Res<> unparse(Io::Emit& emit, Serde::Value const& v) {
    return v.visit(
        Visitor{
            [&](None) -> Res<> {
                emit("null");
                return Ok();
            },
            [&](Serde::Array const& v) -> Res<> {
                emit('[');
                for (usize i = 0; i < v.len(); ++i) {
                    if (i > 0) {
                        emit(',');
                    }
                    try$(unparse(emit, v[i]));
                }
                emit(']');

                return Ok();
            },
            [&](Serde::Object const& m) -> Res<> {
                emit('{');
                bool first = true;
                for (auto const& kv : m.iter()) {
                    if (not first) {
                        emit(',');
                    }
                    first = false;

                    emit('"');
                    emit(kv.v0);
                    emit("\":");
                    try$(unparse(emit, kv.v1));
                }
                emit('}');
                return Ok();
            },
            [&](String const& s) -> Res<> {
                emit('"');
                for (auto c : iterRunes(s)) {
                    if (c == '"') {
                        emit("\\\"");
                    } else if (c == '\\') {
                        emit("\\\\");
                    } else if (c == '\b') {
                        emit("\\b");
                    } else if (c == '\f') {
                        emit("\\f");
                    } else if (c == '\n') {
                        emit("\\n");
                    } else if (c == '\r') {
                        emit("\\r");
                    } else if (c == '\t') {
                        emit("\\t");
                    } else if (c < 0x20) {
                        emit("\\u{x}", c);
                    } else {
                        emit(c);
                    }
                }
                emit('"');
                return Ok();
            },
            [&](Serde::Integer i) -> Res<> {
                emit("{}", i);
                return Ok();
            },
#ifndef __ck_freestanding__
            [&](Serde::Number d) -> Res<> {
                emit("{}", d);
                return Ok();
            },
#endif
            [&](bool b) -> Res<> {
                emit(b ? "true" : "false");
                return Ok();
            },
        }
    );
}

export Res<String> unparse(Serde::Value const& v) {
    Io::StringWriter sw;
    Io::Emit emit{sw};
    try$(unparse(emit, v));
    return Ok(sw.take());
}

} // namespace Karm::Json

export auto operator""_json(char const* str, Karm::usize len) {
    return Karm::Json::parse({str, len}).unwrap();
}
