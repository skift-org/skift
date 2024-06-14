#pragma once

#include "lexer.h"
#include "sst.h"

namespace Web::Css {

Content consumeRuleList(Lexer &lex, bool topLevel);

Sst consumeAtRule(Lexer &lex);

Opt<Sst> consumeRule(Lexer &lex);

Content consumeDeclarationList(Lexer &lex);

Content consumeDeclarationBlock(Lexer &lex);

bool declarationAhead(Lexer lex);

Opt<Sst> consumeDeclaration(Lexer &lex);

Sst consumeComponentValue(Lexer &lex);

Sst consumeBlock(Lexer &lex, Token::Type term);

Sst consumeFunc(Lexer &lex);

} // namespace Web::Css
