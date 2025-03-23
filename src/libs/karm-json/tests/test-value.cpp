#include <karm-json/parse.h>
#include <karm-math/funcs.h>
#include <karm-test/macros.h>

namespace Karm::Json::Tests {

test$("json-value-null") {
    Value value = NONE;

    expectEq$(value, NONE);
    expect$(value.isNull());
    expectEq$(value.asStr(), "null");
    expectEq$(value.asInt(), 0);
    expectEq$(value.asFloat(), 0.0);
    expectEq$(value.asBool(), false);
    expectEq$(value.len(), 0uz);

    return Ok();
}

test$("json-value-array") {
    Value value = Array{
        Integer{1},
        Integer{2},
        Integer{3},
    };

    expect$(not value.isNull());
    expect$(value.isArray());
    expectEq$(value.asStr(), "<array>");
    expectEq$(value.asInt(), 0);
    expectEq$(value.asFloat(), 0.0);
    expectEq$(value.asBool(), true);
    expectEq$(Value{Array{}}.asBool(), false);

    expectEq$(value.len(), 3uz);
    expectEq$(value.get(0).asInt(), 1);
    expectEq$(value.get(1).asInt(), 2);
    expectEq$(value.get(2).asInt(), 3);

    return Ok();
}

test$("json-value-object") {
    Value value = Object{
        {"a"s, Integer{1}},
        {"b"s, Integer{2}},
        {"c"s, Integer{3}},
    };

    expect$(not value.isNull());
    expect$(value.isObject());
    expectEq$(value.asStr(), "<object>");
    expectEq$(value.asInt(), 0);
    expectEq$(value.asFloat(), 0.0);
    expectEq$(value.asBool(), true);
    expectEq$(Value{Object{}}.asBool(), false);

    expectEq$(value.len(), 3uz);
    expectEq$(value.get("a").asInt(), 1);
    expectEq$(value.get("b").asInt(), 2);
    expectEq$(value.get("c").asInt(), 3);

    return Ok();
}

test$("json-value-string") {
    Value value = String{"hello"};

    expect$(value.isStr());
    expectEq$(value.asStr(), "hello");
    expectEq$(value.asInt(), 0);
    expectEq$(value.asFloat(), 0.0);
    expectEq$(value.asBool(), true);
    expectEq$(value.len(), 5uz);

    return Ok();
}

test$("json-value-integer") {
    Value value = Integer{42};

    expect$(value.isInt());
    expectEq$(value.asStr(), "42");
    expectEq$(value.asInt(), 42);
    expectEq$(value.asFloat(), 42.0);
    expectEq$(value.asBool(), true);
    expectEq$(value.len(), 0uz);

    return Ok();
}

test$("json-value-float") {
    Value value = Number{3.14};

    expect$(value.isFloat());
    expect$(Math::epsilonEq(value.asFloat(), 3.14, 0.001));
    expectEq$(value.asStr(), "3.140000"); // FIXME: Once FloatFormatter can stop producing trailing zeros
    expectEq$(value.asInt(), 3);
    expectEq$(value.asBool(), true);
    expectEq$(value.len(), 0uz);

    return Ok();
}

test$("json-value-true") {
    Value value = true;

    expect$(value.isBool());
    expectEq$(value.asStr(), "true");
    expectEq$(value.asInt(), 1);
    expectEq$(value.asFloat(), 1.0);
    expectEq$(value.asBool(), true);
    expectEq$(value.len(), 0uz);

    return Ok();
}

test$("json-value-false") {
    Value value = false;

    expect$(value.isBool());
    expectEq$(value.asStr(), "false");
    expectEq$(value.asInt(), 0);
    expectEq$(value.asFloat(), 0.0);
    expectEq$(value.asBool(), false);
    expectEq$(value.len(), 0uz);

    return Ok();
}

} // namespace Karm::Json::Tests
