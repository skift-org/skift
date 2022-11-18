#include "json.h"

namespace Json {

Result<Value> parse(Text::Scan &s);

Result<String> parseStr(Text::Scan &s) {
    if (!s.skip('"')) {
        return Error{"expected '\"'"};
    }

    s.begin();

    while (!s.ended()) {
        if (s.curr() == '"') {
            auto str = s.end();
            s.next();
            return String{str};
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

        return Error{"invalid string"};
    }

    return Error{"expected '\"'"};
}

Result<Object> parseObject(Text::Scan &s) {
    Object m;
    if (!s.skip('{')) {
        return Error{"expected '{'"};
    }

    if (s.skip('}')) {
        return m;
    }
    while (true) {
        s.eat(Re::space());
        auto key = try$(parseStr(s));

        s.eat(Re::space());
        if (!s.skip(':')) {
            return Error{"expected ':'"};
        }

        s.eat(Re::space());

        auto value = try$(parse(s));
        m.put(key, value);

        s.eat(Re::space());

        if (s.skip('}')) {
            return m;
        }
        if (!s.skip(',')) {
            return Error{"expected ','"};
        }
    }
}

Result<Array> parseArray(Text::Scan &s) {
    Array v;
    if (!s.skip('[')) {
        return Error{"expected '['"};
    }

    if (s.skip(']')) {
        return v;
    }
    while (true) {
        s.eat(Re::space());

        auto value = try$(parse(s));
        v.pushBack(value);

        s.eat(Re::space());

        if (s.skip(']')) {
            return v;
        }
        if (!s.skip(',')) {
            return Error{"expected ','"};
        }
    }
}

Result<Value> parse(Text::Scan &s) {
    s.eat(Re::space());

    if (s.ended()) {
        return Error{"unexpected end of input"};
    } else if (s.peek() == '{') {
        return Value{try$(parseObject(s))};
    } else if (s.peek() == '[') {
        return Value{try$(parseArray(s))};
    } else if (s.peek() == '"') {
        return Value{try$(parseStr(s))};
    } else if (s.skip("null")) {
        return Value{NONE};
    } else if (s.skip("true")) {
        return Value{true};
    } else if (s.skip("false")) {
        return Value{false};
    } else if (s.peek() == '-' || (s.peek() >= '0' && s.peek() <= '9')) {
        return Value{try$(s.nextFloat())};
    }

    return Error{"unexpected character"};
}

Result<Value> parse(Str s) {
    Text::Scan scan{s};
    return parse(scan);
}

} // namespace Json
