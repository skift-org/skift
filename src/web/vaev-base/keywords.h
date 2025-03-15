#pragma once

#include <karm-base/string.h>
#include <karm-io/emit.h>

namespace Vaev {

template <Karm::StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }

    template <Karm::StrLit L>
    constexpr bool operator==(Keyword<L> const&) const {
        return K == L;
    }
};

namespace Keywords {

using Auto = Keyword<"auto">;
static constexpr inline Auto AUTO{};

using Content = Keyword<"content">;
static constexpr inline Content CONTENT{};

using CurrentColor = Keyword<"currentcolor">;
static constexpr inline CurrentColor CURRENT_COLOR{};

using MaxContent = Keyword<"max-content">;
static constexpr inline MaxContent MAX_CONTENT{};

using Medium = Keyword<"medium">;
static constexpr inline Medium MEDIUM{};

using MinContent = Keyword<"min-content">;
static constexpr inline MinContent MIN_CONTENT{};

using None = Keyword<"none">;
static constexpr inline None NONE{};

using Normal = Keyword<"normal">;
static constexpr inline Normal NORMAL{};

using Thick = Keyword<"thick">;
static constexpr inline Thick THICK{};

using Thin = Keyword<"thin">;
static constexpr inline Thin THIN{};

} // namespace Keywords

} // namespace Vaev
