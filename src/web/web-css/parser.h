#pragma once

#include <web-dom/document.h>

#include "karm-logger/logger.h"
#include "lexer.h"

namespace Web::Css {

struct Parser {

    Lexer _lexer;

    Parser() {
    }

    void _raise(Str msg = "parse-error");

    void accept(Token const &t);

    void write(Str str) {
        auto s = Io::SScan(str);
        while (true) {
            auto token = nextToken(s).unwrap();
            logDebug("{#}", token);
            if (token.type == Token::Type::END_OF_FILE or token.data == "") {
                return;
            }
        }
    }
};

} // namespace Web::Css
