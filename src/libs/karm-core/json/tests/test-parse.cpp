#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Json::Tests {

test$("json-parse-null") {
    auto val = "null"_json;
    expect$(val.isNull());
    return Ok();
}

test$("json-parse-array") {
    auto val = "[1, 2, 3]"_json;

    expect$(val.isArray());
    expectEq$(val.len(), 3uz);
    expectEq$(val.get(0).asInt(), 1);
    expectEq$(val.get(1).asInt(), 2);
    expectEq$(val.get(2).asInt(), 3);

    return Ok();
}

test$("json-parse-object") {
    auto val = R"({"a": 1, "b": 2, "c": 3})"_json;

    expect$(val.isObject());
    expectEq$(val.len(), 3uz);
    expectEq$(val.get("a").asInt(), 1);
    expectEq$(val.get("b").asInt(), 2);
    expectEq$(val.get("c").asInt(), 3);

    return Ok();
}

test$("json-parse-string") {
    auto val = R"("hello")"_json;
    expect$(val.isStr());
    expectEq$(val.asStr(), "hello");
    return Ok();
}

test$("json-parse-integer") {
    auto val = "42"_json;
    expect$(val.isInt());
    expectEq$(val.asInt(), 42);
    expectEq$(val.asBool(), true);
    return Ok();
}

test$("json-parse-float") {
    auto val = "3.14"_json;
    expect$(val.isFloat());
    expect$(Math::epsilonEq(val.asFloat(), 3.14, 0.001));
    expectEq$(val.asBool(), true);
    return Ok();
}

test$("json-parse-bool") {
    auto val = "true"_json;
    expect$(val.isBool());
    expectEq$(val.asBool(), true);

    val = "false"_json;
    expect$(val.isBool());
    expectEq$(val.asBool(), false);

    return Ok();
}

} // namespace Karm::Json::Tests
