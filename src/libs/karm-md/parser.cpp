export module Karm.Md:parser;

import Karm.Core;

import :base;

namespace Karm::Md {

void _eatWhitespace(Io::SScan& s) {
    s.eat(Re::space());
}

Node _parseSpan(Io::SScan& s) {
    s.begin();
    while (not s.ahead("\n\n") and not s.ended()) {
        s.next();
    }
    return String{s.end()};
}

Paragraph _parseParagraph(Io::SScan& s) {
    Paragraph p;
    while (not s.ahead('\n') and not s.ended()) {
        p.pushBack(_parseSpan(s));
    }
    return p;
}

Heading _parseHeading(Io::SScan& s) {
    usize level = 0;
    while (s.peek() == '#') {
        level++;
        s.next();
    }

    return Heading{
        .level = level,
        .children = _parseParagraph(s),
    };
}

Node _parseCode(Io::SScan& s) {
    if (not s.skip("```"))
        return _parseParagraph(s);
    s.begin();
    while (not s.ended()) {
        if (s.ahead("```")) {
            auto code = s.end();
            s.skip("```");
            return Code{code};
        }
        s.next();
    }
    return Code{s.end()};
}

auto RE_HR =
    (Re::atLeast(3, '*'_re) |
     Re::atLeast(3, '-'_re) |
     Re::atLeast(3, '_'_re)) &
    '\n'_re;

Node _parseBlock(Io::SScan& s) {
    if (s.peek(0) == '#')
        return _parseHeading(s);
    else if (s.peek(0) == '`')
        return _parseCode(s);
    else if (s.skip(RE_HR))
        return Hr{};
    else
        return _parseParagraph(s);
}

Document _parseDocument(Io::SScan& s) {
    Vec<Node> children;
    _eatWhitespace(s);
    while (not s.ended()) {
        children.pushBack(_parseBlock(s));
        _eatWhitespace(s);
    }
    return Document{children};
}

export Document parse(Io::SScan& s) {
    return _parseDocument(s);
}

export Document parse(Str str) {
    Io::SScan s{str};
    return parse(s);
}

} // namespace Karm::Md
