#include <karm-test/macros.h>

import Vaev.Engine;

using namespace Karm;

namespace Vaev::Css::Tests {

test$("vaev-css-parse-func") {
    Lexer lex{"func(1 2 3 4) not-consumed(4 3 2 1)"};
    auto sst = consumeFunc(lex);
    expect$(not lex.ended());

    expectEq$(sst, Sst::FUNC);

    // check the prefix
    expectEq$(sst.prefix, Sst::TOKEN);
    expectEq$(sst.prefix, Token::FUNCTION);

    // check the content
    expectEq$(sst.content.len(), 7uz);
    expectEq$(sst.content[0], Token::NUMBER);
    expectEq$(sst.content[1], Token::WHITESPACE);
    expectEq$(sst.content[2], Token::NUMBER);
    expectEq$(sst.content[3], Token::WHITESPACE);
    expectEq$(sst.content[4], Token::NUMBER);
    expectEq$(sst.content[5], Token::WHITESPACE);
    expectEq$(sst.content[6], Token::NUMBER);

    return Ok();
}

} // namespace Vaev::Css::Tests
