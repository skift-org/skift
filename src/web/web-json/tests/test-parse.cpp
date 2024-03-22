#include <karm-math/funcs.h>
#include <karm-test/macros.h>
#include <web-json/json.h>

namespace Web::Json::Tests {

test$(jsonNull) {
    auto val = "null"_json;
    expect$(val.isNull());
    return Ok();
}

test$(jsonArray) {
    auto val = "[1, 2, 3]"_json;

    expect$(val.isArray());
    expectEq$(val.len(), 3uz);
    expectEq$(val.get(0).asInt(), 1);
    expectEq$(val.get(1).asInt(), 2);
    expectEq$(val.get(2).asInt(), 3);

    return Ok();
}

test$(jsonObject) {
    auto val = R"({"a": 1, "b": 2, "c": 3})"_json;

    expect$(val.isObject());
    expectEq$(val.len(), 3uz);
    expectEq$(val.get("a").asInt(), 1);
    expectEq$(val.get("b").asInt(), 2);
    expectEq$(val.get("c").asInt(), 3);

    return Ok();
}

test$(jsonString) {
    auto val = R"("hello")"_json;
    expect$(val.isStr());
    expectEq$(val.asStr(), "hello");
    return Ok();
}

test$(jsonInteger) {
    auto val = "42"_json;
    expect$(val.isInt());
    expectEq$(val.asInt(), 42);
    expectEq$(val.asBool(), true);
    return Ok();
}

test$(jsonFloat) {
    auto val = "3.14"_json;
    expect$(val.isFloat());
    expect$(Math::epsilonEq(val.asFloat(), 3.14, 0.0001));
    expectEq$(val.asBool(), true);
    return Ok();
}

test$(jsonBool) {
    auto val = "true"_json;
    expect$(val.isBool());
    expectEq$(val.asBool(), true);

    val = "false"_json;
    expect$(val.isBool());
    expectEq$(val.asBool(), false);

    return Ok();
}

} // namespace Web::Json::Tests
