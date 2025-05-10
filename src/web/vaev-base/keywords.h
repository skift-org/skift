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

using MarginBox = Keyword<"margin-box">;
static constexpr inline MarginBox MARGIN_BOX{};

using BorderBox = Keyword<"border-box">;
static constexpr inline BorderBox BORDER_BOX{};

using PaddingBox = Keyword<"padding-box">;
static constexpr inline PaddingBox PADDING_BOX{};

using ContentBox = Keyword<"content-box">;
static constexpr inline ContentBox CONTENT_BOX{};

using FillBox = Keyword<"fill-box">;
static constexpr inline FillBox FILL_BOX{};

using StrokeBox = Keyword<"stroke-box">;
static constexpr inline StrokeBox STROKE_BOX{};

using ViewBox = Keyword<"view-box">;
static constexpr inline ViewBox VIEW_BOX{};

using Nonzero = Keyword<"nonzero">;
static constexpr inline Nonzero NONZERO{};

using Evenodd = Keyword<"evenodd">;
static constexpr inline Evenodd EVENODD{};

using Left = Keyword<"left">;
static constexpr inline Left LEFT{};

using Right = Keyword<"right">;
static constexpr inline Right RIGHT{};

using Top = Keyword<"top">;
static constexpr inline Top TOP{};

using Bottom = Keyword<"bottom">;
static constexpr inline Bottom BOTTOM{};

using Center = Keyword<"center">;
static constexpr inline Center CENTER{};

using ClosestSide = Keyword<"closest-side">;
static constexpr inline ClosestSide CLOSEST_SIDE{};

using FarthestSide = Keyword<"farthest-side">;
static constexpr inline FarthestSide FARTHEST_SIDE{};

} // namespace Keywords

} // namespace Vaev
