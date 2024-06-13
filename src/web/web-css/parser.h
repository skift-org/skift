#pragma once

#include "lexer.h"
#include "sst.h"

namespace Web::Css {

Content consumeRuleList(Lexer &lex, bool topLevel);

Sst consumeAtRule(Lexer &lex);

Opt<Sst> consumeQualifiedRule(Lexer &lex);

Content consumeStyleBlock(Lexer &lex);

Sst consumeDeclarationList(Lexer &lex);

Opt<Sst> consumeDeclaration(Lexer &lex);

Sst consumeComponentValue(Lexer &lex);

Sst consumeBlock(Lexer &lex, Token::Type term);

Sst consumeFunc(Lexer &lex);

} // namespace Web::Css
