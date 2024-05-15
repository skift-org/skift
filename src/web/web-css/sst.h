#pragma once

#include <karm-io/funcs.h>

#include "lexer.h"

// The SST (Skeleton Syntax Tree) is an intermediary representation of the CSS used to build the real syntaxic tree
// We have all the block and declarations here but didn't interpreted it because we lacked context in the previous parse step
// when we have this representation we can parse it a last time and interpret the different blocks and functions to build the CSSOM
// The name come from: https://people.csail.mit.edu/jrb/Projects/dexprs.pdf chapter 3.4

namespace Web::Css {

struct Sst;

using Content = Vec<Sst>;

#define FOREACH_SST(SST) \
    SST(QUALIFIED_RULE)  \
    SST(FUNC)            \
    SST(DECL)            \
    SST(LIST)            \
    SST(TOKEN)           \
    SST(BLOCK)

struct Sst {
    enum struct _Type {
#define ITER(NAME) NAME,
        FOREACH_SST(ITER)
#undef ITER
    };
    using enum _Type;

    _Type type;
    Opt<Token> token{};
    Opt<Box<Sst>> prefix{};
    Content content{};

    Sst(_Type type) : type(type) {}
};
} // namespace Web::Css
