#pragma once

#include <vaev-css/parser.h>

namespace Vaev::Style {

static void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

} // namespace Vaev::Style
