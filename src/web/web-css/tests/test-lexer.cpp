#include <karm-io/sscan.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>
#include <web-css/lexer.h>

namespace Web::Css::Tests {

Token nextToken(Io::SScan &s) {
    return Lexer{s}.peek();
}

test$(comments) {
    auto s = Io::SScan("/* comment */");
    auto t = nextToken(s);
    expectEq$(t.type, Token::COMMENT);
    expectEq$(t.data, "/* comment */");

    s = Io::SScan("/* unterminated comment");
    auto t2 = nextToken(s);
    expectEq$(t2.type, Token::COMMENT);
    expectEq$(t2.data, "/* unterminated comment");

    return Ok();
}

test$(numbers) {
    auto s = Io::SScan("123");
    auto t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123");

    s = Io::SScan("123%");
    t = nextToken(s);
    expectEq$(t.type, Token::PERCENTAGE);
    expectEq$(t.data, "123%");

    s = Io::SScan("123.456");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456");

    s = Io::SScan("123.456e7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456e7");

    s = Io::SScan("123.456E7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("123.456E7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("-123.456E7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "-123.456E7");

    s = Io::SScan("123.456E7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("123.456E-7");
    t = nextToken(s);
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E-7");

    s = Io::SScan("123px");
    t = nextToken(s);
    expectEq$(t.type, Token::DIMENSION);
    expectEq$(t.data, "123px");

    s = Io::SScan("123.456px");
    t = nextToken(s);
    expectEq$(t.type, Token::DIMENSION);
    expectEq$(t.data, "123.456px");

    s = Io::SScan("123.456e7px");
    t = nextToken(s);
    expectEq$(t.type, Token::DIMENSION);

    s = Io::SScan("123.456E7px");
    t = nextToken(s);
    expectEq$(t.type, Token::DIMENSION);

    s = Io::SScan("+123.456E7px");
    t = nextToken(s);
    expectEq$(t.type, Token::DIMENSION);

    return Ok();
}

test$(strings) {
    auto s = Io::SScan("''");
    auto t = nextToken(s);
    expectEq$(t.type, Token::STRING);

    s = Io::SScan("\"\"");
    t = nextToken(s);
    expectEq$(t.type, Token::STRING);

    s = Io::SScan("\"abc\"");
    t = nextToken(s);
    expectEq$(t.type, Token::STRING);

    s = Io::SScan("'abc'");
    t = nextToken(s);
    expectEq$(t.type, Token::STRING);

    s = Io::SScan("' Hello World !'");
    t = nextToken(s);
    expectEq$(t.type, Token::STRING);

    return Ok();
}
}; // namespace Web::Css::Tests
