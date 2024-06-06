#pragma once

#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-sys/file.h>
#include <web-cssom/stylesheet.h>

#include "builder.h"
#include "lexer.h"

namespace Web::Css {

Res<Sst> consumeBlock(Io::SScan &s, Token::Type term);

Res<Sst> consumeFunc(Io::SScan &s, Token token);

Res<Sst> consumeComponentValue(Io::SScan &s, Token token);

Res<Sst> consumeQualifiedRule(Io::SScan &s, Token token);

Res<Sst> consumeRuleList(Io::SScan &s);

} // namespace Web::Css
