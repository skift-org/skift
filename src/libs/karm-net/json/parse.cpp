#include <karm-io/aton.h>
#include <karm-io/funcs.h>

#include "json.h"

namespace Karm::Net::Json {

Res<Value> parse(Io::SScan &s);

Res<String> parseStr(Io::SScan &s) {
    if (not s.skip('"'))
        return Error::invalidData("expected '\"'");

    s.begin();

    while (not s.ended()) {
        if (s.curr() == '"') {
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

Res<Object> parseObject(Io::SScan &s) {
    Object m;
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

Res<Array> parseArray(Io::SScan &s) {
    Array v;
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

Res<usize> parseDigits(Io::SScan &s) {
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

Res<f64> parseDecimal(Io::SScan &s) {
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

Res<isize> parseInteger(Io::SScan &s) {
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

Res<Value> parseNumber(Io::SScan &s) {
    isize ipart = try$(parseInteger(s));

    if (s.match(Re::single('.', 'e', 'E')) == Match::NO)
        return Ok<Value>(ipart);

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

        return Ok<Value>(ipart + fpart * pow(10, exp));
    }

    return Ok<Value>(ipart + fpart);
#endif
}

Res<Value> parse(Io::SScan &s) {
    s.eat(Re::space());

    if (s.ended()) {
        return Error::invalidData("unexpected end of input");
    } else if (s.peek() == '{') {
        return Ok(Value{try$(parseObject(s))});
    } else if (s.peek() == '[') {
        return Ok(Value{try$(parseArray(s))});
    } else if (s.peek() == '"') {
        return Ok(Value{try$(parseStr(s))});
    } else if (s.skip("null")) {
        return Ok(Value{NONE});
    } else if (s.skip("true")) {
        return Ok(Value{true});
    } else if (s.skip("false")) {
        return Ok(Value{false});
    } else if (s.match(RE_NUMBER_START) != Match::NO) {
        return parseNumber(s);
    }

    return Error::invalidData("unexpected character");
}

Res<Value> parse(Str s) {
    Io::SScan scan{s};
    return parse(scan);
}

} // namespace Karm::Net::Json
