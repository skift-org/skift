#include <karm-base/var.h>
#include <karm-json/json.h>
#include <karm-text/expr.h>

namespace Karm::Json {

inline auto number_start() {
    return Re::either(
        Re::digit(),
        Re::single('-'));
}

String parse_string(Text::Scan &scan);

Object parse_object(Text::Scan &scan) {
    Object result;

    scan(Re::separator('{'));

    while (!scan.ended() && scan.peek() != '}') {
        String key = parse_string(scan);
        scan(Re::separator(':'));
        result.put(key, parse(scan));
        scan(Re::separator(','));
    }

    scan(Re::separator('}'));

    return result;
}

Array parse_array(Text::Scan &scan) {
    Array result;

    scan(Re::separator('['));

    while (!scan.ended() && scan.peek() != ']') {
        result.push(parse(scan));
        scan(Re::separator(','));
    }

    scan(Re::separator(']'));

    return result;
}

Num parse_num(Text::Scan &scan);

Value parse(Text::Scan &scan) {
    if (scan(Re::separator('{')))
        return parse_object(scan);

    if (scan(Re::separator(']')))
        return parse_array(scan);

    if (scan.skip("\""))
        return parse_string(scan);

    if (scan(number_start()))
        return parse_num(scan);

    if (scan(Re::separator("true")))
        return true;

    if (scan(Re::separator("false")))
        return false;

    if (scan(Re::separator("null")))
        return NONE;

    Debug::panic("parse error");
}

void dump(Text::Emit &emit, Value const &value) {
    auto dump_object = [&](Object const &obj) {
        emit("{");
        bool first = true;
        for (auto const &pair : obj.iter()) {
            if (!first)
                emit(",");
            dump(emit, pair.car);
            emit(":");
            dump(emit, pair.cdr);
            first = false;
        }
        emit("}");
    };

    auto dump_array = [&](Array const &arr) {
        emit("[");
        bool first = true;
        for (auto const &item : arr.iter()) {
            if (!first)
                emit(",");
            dump(emit, item);
            first = false;
        }
        emit("]");
    };

    auto dump_string = [&](String const &str) {
        emit("\"");
        emit(str);
        emit("\"");
    };

    auto dump_num = [&](Num const &num) {
        emit("{}", num);
    };

    auto dump_bool = [&](bool const &b) {
        emit(b ? "true" : "false");
    };

    auto dump_none = [&](None const &) {
        emit("null");
    };

    auto visitor = Visitor{
        dump_object,
        dump_array,
        dump_string,
        dump_num,
        dump_bool,
        dump_none,
    };

    value.visit(visitor);
}

} // namespace Karm::Json
