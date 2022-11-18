#include <json/json.h>
#include <karm-main/main.h>

CliResult entryPoint(CliArgs) {
    Json::Value v = try$(Json::parse(R"(
        {
            "hello": "world",
            "foo": "bar",
            "baz": 123,
            "qux": true,
            "quux": null,
            "corge": [1, 2, 3],
            "grault": {
                "garply": "waldo",
                "fred": "plugh",
                "xyzzy": "thud"
            }
        }
    )"));

    Sys::println("v = {}", v);
    return OK;
}
