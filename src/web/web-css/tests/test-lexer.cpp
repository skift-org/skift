#include <karm-io/sscan.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>
#include <web-css/lexer.h>

namespace Web::Css::Tests {

test$(comments) {
    auto s = Io::SScan("/* comment */");
    auto t = try$(nextToken(s));
    expect$(t.type == Token::COMMENT);
    expectEq$(t.data, "/* comment */");

    s = Io::SScan("/* unterminated comment");
    auto t2 = nextToken(s);
    expect$(not t2.has());
    expectEq$(t2.none(), Error::invalidInput("unterminated comment"));
    return Ok();
}

test$(numbers) {
    auto s = Io::SScan("123");
    auto t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123");

    s = Io::SScan("123%");
    t = try$(nextToken(s));
    expect$(t.type == Token::PERCENTAGE);
    expectEq$(t.data, "123%");

    s = Io::SScan("123.456");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456");

    s = Io::SScan("123.456e7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456e7");

    s = Io::SScan("123.456E7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("123.456E7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("-123.456E7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "-123.456E7");

    s = Io::SScan("123.456E7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456E7");

    s = Io::SScan("123.456E-7");
    t = try$(nextToken(s));
    expect$(t.type == Token::NUMBER);
    expectEq$(t.data, "123.456E-7");

    s = Io::SScan("123px");
    t = try$(nextToken(s));
    expect$(t.type == Token::DIMENSION);
    expectEq$(t.data, "123px");

    s = Io::SScan("123.456px");
    t = try$(nextToken(s));
    expect$(t.type == Token::DIMENSION);
    expectEq$(t.data, "123.456px");

    s = Io::SScan("123.456e7px");
    t = try$(nextToken(s));
    expect$(t.type == Token::DIMENSION);

    s = Io::SScan("123.456E7px");
    t = try$(nextToken(s));
    expect$(t.type == Token::DIMENSION);

    s = Io::SScan("+123.456E7px");
    t = try$(nextToken(s));
    expect$(t.type == Token::DIMENSION);

    return Ok();
}

test$(strings) {
    auto s = Io::SScan("''");
    auto t = try$(nextToken(s));
    expect$(t.type == Token::STRING);

    s = Io::SScan("\"\"");
    t = try$(nextToken(s));
    expect$(t.type == Token::STRING);

    s = Io::SScan("\"abc\"");
    t = try$(nextToken(s));
    expect$(t.type == Token::STRING);

    s = Io::SScan("'abc'");
    t = try$(nextToken(s));
    expect$(t.type == Token::STRING);

    s = Io::SScan("' Hello World !'");
    t = try$(nextToken(s));
    expect$(t.type == Token::STRING);

    return Ok();
}
}; // namespace Web::Css::Tests
