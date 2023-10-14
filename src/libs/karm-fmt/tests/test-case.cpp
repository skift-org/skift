#include <karm-fmt/case.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

struct {
    Fmt::Case c;
    Str input;
    Str expected;
} CASES[] = {
    {Fmt::Case::NO, "noCase", "no case"},
    {Fmt::Case::NO, "no_case", "no case"},
    {Fmt::Case::NO, "NO_CASE", "no case"},
    {Fmt::Case::NO, "no/case", "no case"},
    {Fmt::Case::NO, "no.case", "no case"},
    {Fmt::Case::NO, "No Case", "no case"},
    {Fmt::Case::DEFAULT, "Default Case", "Default Case"},
    {Fmt::Case::DEFAULT, "this is unchanged", "this is unchanged"},
    {Fmt::Case::DEFAULT, "this.is/unchanged", "this.is/unchanged"},
    {Fmt::Case::CAMEL, "camel case", "camelCase"},
    {Fmt::Case::CAPITAL, "capitalCase", "Capital Case"},
    {Fmt::Case::CONSTANT, "hello world", "HELLO_WORLD"},
    {Fmt::Case::DOT, "hello world", "hello.world"},
    {Fmt::Case::HEADER, "HelloWorld", "Hello-World"},
    {Fmt::Case::PARAM, "hello world", "hello-world"},
    {Fmt::Case::PASCAL, "hello world", "HelloWorld"},
    {Fmt::Case::PATH, "hello world", "hello/world"},
    {Fmt::Case::SENTENCE, "Hello World", "Hello world"},
    {Fmt::Case::SNAKE, "Hello World", "hello_world"},
    {Fmt::Case::TITLE, "hello world", "Hello World"},
    {Fmt::Case::SWAP, "Hello World", "hELLO wORLD"},
    {Fmt::Case::LOWER, "HELLO WORLD", "hello world"},
    {Fmt::Case::LOWER_FIRST, "HELLO WORLD", "hELLO WORLD"},
    {Fmt::Case::UPPER, "helloWorld", "HELLOWORLD"},
    {Fmt::Case::UPPER, "hello world", "HELLO WORLD"},
    {Fmt::Case::UPPER_FIRST, "hello world", "Hello world"},
    {Fmt::Case::SPONGE, "hello world", "HeLlO WoRlD"},
    {Fmt::Case::SPONGE, "NFTs are the future of the web", "NfTs aRe tHe fUtUrE Of tHe wEb"},
};

test$(changeChase) {
    for (auto [c, input, expected] : CASES) {
        auto result = try$(Fmt::changeCase(input, c));
        // logInfo("input: {}, expected: {}, result: {}", input, expected, result);
        expectEq$(result, expected);
    }

    return Ok();
}
