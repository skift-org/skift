#pragma once

#include <web-dom/document.h>

#include "karm-base/slice.h"
#include "karm-logger/logger.h"
#include "lexer.h"

namespace Web::Css {

struct Parser : public Sink {

    Lexer _lexer;

    Parser() {
        _lexer.bind(*this);
    }

    void _raise(Str msg = "parse-error");

    void accept(Token const &t) override;

    void write(Str str) {
        auto s = Io::SScan(str);
        while (true) {
            auto t = nextToken(s);
            logDebug("{#}", t);
            if (t.unwrap().type == Token::Type::END_OF_FILE || t.unwrap().data == "") {
                return;
            }
        }
    }
};

} // namespace Web::Css
