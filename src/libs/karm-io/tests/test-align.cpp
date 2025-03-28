#include <karm-io/fmt.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

struct {
    Align a;
    int width;
    Str input;
    Str expected;
} CASES[] = {
    {Align::LEFT, 9, "hello", "hello    "},
    {Align::CENTER, 9, "hello", "  hello  "},
    {Align::RIGHT, 9, "hello", "    hello"},
};

test$("fmt-align") {
    for (auto [a, width, input, expected] : CASES) {
        auto result = format("{}", aligned(input, a, width));
        expectEq$(result, expected);
    }

    return Ok();
}

} // namespace Karm::Io::Tests
