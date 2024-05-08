#pragma once

#include <karm-io/funcs.h>

#include "lexer.h"

namespace Web::Css {

struct Ast;

using Content = Vec<Ast>;

#define FOREACH_AST(AST) \
    AST(QUALIFIED_RULE)  \
    AST(FUNC)            \
    AST(DECL)            \
    AST(LIST)            \
    AST(TOKEN)           \
    AST(BLOCK)

struct Ast {
    enum struct _Type {
#define ITER(NAME) NAME,
        FOREACH_AST(ITER)
#undef ITER
    };

    using enum _Type;

    _Type type;
    Opt<Token> token{};
    Opt<Box<Ast>> prefix{};
    Content content{};

    Ast(_Type type) : type(type) {}
};
} // namespace Web::Css
