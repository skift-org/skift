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

using First = Keyword<"first">;
static constexpr inline First FIRST{};

using Last = Keyword<"last">;
static constexpr inline Last LAST{};

using Baseline = Keyword<"baseline">;
static constexpr inline Baseline BASELINE{};

using TextBottom = Keyword<"text-bottom">;
static constexpr inline TextBottom TEXT_BOTTOM{};

using Alphabetic = Keyword<"alphabetic">;
static constexpr inline Alphabetic ALPHABETIC{};

using Ideographic = Keyword<"ideographic">;
static constexpr inline Ideographic IDEOGRAPHIC{};

using Middle = Keyword<"middle">;
static constexpr inline Middle MIDDLE{};

using Central = Keyword<"central">;
static constexpr inline Central CENTRAL{};

using Mathematical = Keyword<"mathematical">;
static constexpr inline Mathematical MATHEMATICAL{};

using TextTop = Keyword<"text-top">;
static constexpr inline TextTop TEXT_TOP{};

} // namespace Keywords

} // namespace Vaev
