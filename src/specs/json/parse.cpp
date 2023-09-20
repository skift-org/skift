#include <karm-io/funcs.h>

#include "json.h"

namespace Json {

Res<Value> parse(Io::SScan &s);

Res<String> parseStr(Io::SScan &s) {
    if (not s.skip('"')) {
        return Error::invalidData("expected '\"'");
    }

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
                if (s.nextUint(16)) {
                    continue;
                }
            }
        }

        if (s.next()) {
            continue;
        }

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
        if (not s.skip(':')) {
            return Error::invalidData("expected ':'");
        }

        s.eat(Re::space());

        auto value = try$(parse(s));
        m.put(key, value);

        s.eat(Re::space());

        if (s.skip('}')) {
            return Ok(m);
        }
        if (not s.skip(',')) {
            return Error::invalidData("expected ','");
        }
    }
}

Res<Array> parseArray(Io::SScan &s) {
    Array v;
    if (not s.skip('[')) {
        return Error::invalidData("expected '['");
    }

    if (s.skip(']')) {
        return Ok(v);
    }
    while (true) {
        s.eat(Re::space());

        auto value = try$(parse(s));
        v.pushBack(value);

        s.eat(Re::space());

        if (s.skip(']')) {
            return Ok(v);
        }
        if (not s.skip(',')) {
            return Error::invalidData("expected ','");
        }
    }
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
    } else if (s.peek() == '-' or (s.peek() >= '0' and s.peek() <= '9')) {
#ifdef __ck_freestanding__
        return Ok(Value{(Number)try$(s.nextInt())});
#else
        return Ok(Value{try$(s.nextFloat())});
#endif
    }

    return Error::invalidData("unexpected character");
}

Res<Value> parse(Str s) {
    Io::SScan scan{s};
    return parse(scan);
}

} // namespace Json
