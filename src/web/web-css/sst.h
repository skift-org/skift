#pragma once

#include <karm-io/emit.h>

#include "lexer.h"

// The SST (Skeleton Syntax Tree) is an intermediary representation of the CSS used to build the real syntaxic tree
// We have all the block and declarations here but didn't interpreted it because we lacked context in the previous parse step
// when we have this representation we can parse it a last time and interpret the different blocks and functions to build the CSSOM
// The name come from: https://people.csail.mit.edu/jrb/Projects/dexprs.pdf chapter 3.4

namespace Web::Css {

struct Sst;

using Content = Vec<Sst>;

#define FOREACH_SST(SST) \
    SST(RULE)            \
    SST(FUNC)            \
    SST(DECL)            \
    SST(LIST)            \
    SST(TOKEN)           \
    SST(BLOCK)

struct Sst {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_SST(ITER)
#undef ITER
    };
    using enum Type;

    Type type;
    Token token = Token(Token::NIL);
    Opt<Box<Sst>> prefix{};
    Content content{};

    Sst(Type type) : type(type) {}

    Sst(Token token) : type(TOKEN), token(token) {}

    Sst(Content content) : type(LIST), content(content) {}

    void repr(Io::Emit &e) const;

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token::Type const &type) const {
        return this->type == TOKEN and
               token.type == type;
    }
};

Str toStr(Sst::Type type);

} // namespace Web::Css

Reflectable$(Web::Css::Sst, type, token, prefix, content);

template <>
struct Karm::Io::Formatter<Web::Css::Sst::Type> {
    Res<usize> format(Io::TextWriter &writer, Web::Css::Sst::Type val) {
        return (writer.writeStr(try$(Io::toParamCase(toStr(val)))));
    }
};
