export module Vaev.Engine:values.keywords;

import Karm.Core;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

export template <StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }

    template <Karm::StrLit L>
    constexpr bool operator==(Keyword<L> const&) const {
        return K == L;
    }
};

export template <StrLit K>
struct ValueParser<Keyword<K>> {
    static Res<Keyword<K>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->token == Css::Token::ident(K)) {
            c.next();
            return Ok(Keyword<K>{});
        }

        return Error::invalidData("expected keyword");
    }
};

namespace Keywords {

#define KEYWORD(TYPE, VALUE, NAME)      \
    export using TYPE = Keyword<VALUE>; \
    export constexpr inline TYPE NAME{};
#include "defs/keywords.inc"
#undef KEYWORD

} // namespace Keywords

} // namespace Vaev
