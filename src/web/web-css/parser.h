#pragma once

#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/file.h>
#include <web-style/stylesheet.h>

#include "builder.h"
#include "lexer.h"

namespace Web::Css {

Sst consumeBlock(Lexer &lex, Token::Type term);

Sst consumeFunc(Lexer &lex);

Sst consumeComponentValue(Lexer &lex);

Sst consumeQualifiedRule(Lexer &lex);

Sst consumeRuleList(Lexer &lex);

} // namespace Web::Css
