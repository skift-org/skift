#include <karm-test/macros.h>
import Karm.Core;

namespace Karm::Io::Tests {

struct {
    Case c;
    Str input;
    Str expected;
} CASES[] = {
    {Case::NO, "noCase", "no case"},
    {Case::NO, "no_case", "no case"},
    {Case::NO, "NO_CASE", "no case"},
    {Case::NO, "no/case", "no case"},
    {Case::NO, "no.case", "no case"},
    {Case::NO, "No Case", "no case"},
    {Case::DEFAULT, "Default Case", "Default Case"},
    {Case::DEFAULT, "this is unchanged", "this is unchanged"},
    {Case::DEFAULT, "this.is/unchanged", "this.is/unchanged"},
    {Case::CAMEL, "camel case", "camelCase"},
    {Case::CAPITAL, "capitalCase", "Capital Case"},
    {Case::CONSTANT, "hello world", "HELLO_WORLD"},
    {Case::DOT, "hello world", "hello.world"},
    {Case::HEADER, "HelloWorld", "Hello-World"},
    {Case::PARAM, "hello world", "hello-world"},
    {Case::PASCAL, "hello world", "HelloWorld"},
    {Case::PATH, "hello world", "hello/world"},
    {Case::SENTENCE, "Hello World", "Hello world"},
    {Case::SNAKE, "Hello World", "hello_world"},
    {Case::TITLE, "hello world", "Hello World"},
    {Case::SWAP, "Hello World", "hELLO wORLD"},
    {Case::LOWER, "HELLO WORLD", "hello world"},
    {Case::LOWER_FIRST, "HELLO WORLD", "hELLO WORLD"},
    {Case::UPPER, "helloWorld", "HELLO WORLD"},
    {Case::UPPER, "hello world", "HELLO WORLD"},
    {Case::UPPER_FIRST, "hello world", "Hello world"},
    {Case::SPONGE, "hello world", "HeLlO wOrLd"},
    {Case::SPONGE, "NFTs are the future of the web", "NfTs ArE tHe FuTuRe Of ThE wEb"},
};

test$("case-change") {
    for (auto [c, input, expected] : CASES) {
        auto result = try$(changeCase(input, c));
        expectEq$(result, expected);
    }

    return Ok();
}

} // namespace Karm::Io::Tests
