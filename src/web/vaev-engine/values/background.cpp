module;

#include <karm-core/macros.h>

export module Vaev.Engine:values.background;

import Karm.Core;

import :css;
import :values.base;
import :values.calc;
import :values.color;
import :values.image;
import :values.keywords;
import :values.length;
import :values.percent;

using namespace Karm;

namespace Vaev {

export enum struct BackgroundAttachment {
    SCROLL,
    FIXED,
    LOCAL,

    _LEN
};

// MARK: Background Position ---------------------------------------------------
// https://www.w3.org/TR/css-backgrounds-3/#typedef-bg-position
// [
//   case 1 :
//   [ left | center | right | top | bottom | <length-percentage> ]
// |
//   case 2 :
//   [ left | center | right | <length-percentage> ]
//   [ top | center | bottom | <length-percentage> ]
// |
//   case 3 :
//   [ center | [ left | right ] <length-percentage>? ] &&
//   [ center | [ top | bottom ] <length-percentage>? ]
// ]

export struct BackgroundPosition {
    using HorizontalAnchor = Union<Keywords::Left, Keywords::Center, Keywords::Right>;
    using VerticalAnchor = Union<Keywords::Top, Keywords::Center, Keywords::Bottom>;

    HorizontalAnchor horizontalAnchor;
    CalcValue<PercentOr<Length>> horizontal;
    VerticalAnchor verticalAnchor;
    CalcValue<PercentOr<Length>> vertical;

    constexpr BackgroundPosition()
        : horizontalAnchor(Keywords::LEFT),
          horizontal(Percent{0}),
          verticalAnchor(Keywords::TOP),
          vertical(Percent{0}) {
    }

    constexpr BackgroundPosition(CalcValue<PercentOr<Length>> horizontal, CalcValue<PercentOr<Length>> vertical)
        : horizontalAnchor(Keywords::LEFT), horizontal(horizontal), verticalAnchor(Keywords::TOP), vertical(vertical) {
    }

    constexpr BackgroundPosition(HorizontalAnchor horizontalAnchor, CalcValue<PercentOr<Length>> horizontal, VerticalAnchor verticalAnchor, CalcValue<PercentOr<Length>> vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }

    void repr(Io::Emit& e) const {
        e("({} {}", horizontalAnchor, horizontal);

        e(" ");
        e("{} {})", verticalAnchor, vertical);
    }
};

export template <>
struct ValueParser<BackgroundPosition> {
    static Res<BackgroundPosition> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        using Item = FlatUnion<BackgroundPosition::HorizontalAnchor, BackgroundPosition::VerticalAnchor, CalcValue<PercentOr<Length>>>;

        InlineVec<Item, 4> items;
        for (int i = 0; i < 4 && not c.ended(); ++i) {
            eatWhitespace(c);
            auto item = parseValue<Item>(c);
            eatWhitespace(c);
            if (item)
                items.pushBack(item.unwrap());
            else
                break;
        }

        if (not items)
            return Error::invalidData("invalid background position");

        if (items.len() == 1) { // case 1
            if (items[0].is<Keywords::Bottom>() or items[0].is<Keywords::Top>())
                items.pushFront(Keywords::CENTER);
            else
                items.pushBack(Keywords::CENTER);
        }

        if (items.len() == 2) { // case 2
            if (items[0].is<Keywords::Center>() and (items[1].is<Keywords::Left>() or items[1].is<Keywords::Right>()))
                items.pushBack(items.popFront());

            BackgroundPosition::HorizontalAnchor hAnchor = Keywords::LEFT;
            CalcValue<PercentOr<Length>> hValue = {Percent(0)};
            BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
            CalcValue<PercentOr<Length>> vValue = {Percent(0)};

            try$(items[0].visit(Visitor{
                [&](Meta::Contains<Keywords::Left, Keywords::Right, Keywords::Center> auto& t) -> Res<> {
                    hAnchor = t;
                    return Ok();
                },
                [&](CalcValue<PercentOr<Length>>& t) -> Res<> {
                    hValue = t;
                    return Ok();
                },
                [](auto&) -> Res<> {
                    return Error::invalidData("invalid horizontal anchor");
                },
            }));

            try$(items[1].visit(Visitor{
                [&](Meta::Contains<Keywords::Bottom, Keywords::Top, Keywords::Center> auto& t) -> Res<> {
                    vAnchor = t;
                    return Ok();
                },
                [&](CalcValue<PercentOr<Length>>& t) -> Res<> {
                    vValue = t;
                    return Ok();
                },
                [](auto&) -> Res<> {
                    return Error::invalidData("invalid vertical anchor");
                },
            }));

            return Ok(BackgroundPosition(hAnchor, hValue, vAnchor, vValue));
        }

        // case 3
        BackgroundPosition::HorizontalAnchor hAnchor = Keywords::LEFT;
        CalcValue<PercentOr<Length>> hValue = {Percent(0)};
        bool hSet = false;

        BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
        CalcValue<PercentOr<Length>> vValue = {Percent(0)};

        usize secondPairIndex = 2;

        try$(items[0].visit(Visitor{
            [&](Meta::Contains<Keywords::Left, Keywords::Right> auto& t) -> Res<> {
                hAnchor = t;
                hSet = true;

                if (auto mesure = items[1].is<CalcValue<PercentOr<Length>>>()) {
                    hValue = *mesure;
                } else {
                    secondPairIndex = 1;
                }

                return Ok();
            },
            [&](Keywords::Center& t) -> Res<> {
                hAnchor = t;
                hSet = true;

                secondPairIndex = 1;

                return Ok();
            },
            [&](Meta::Contains<Keywords::Top, Keywords::Bottom> auto& t) -> Res<> {
                vAnchor = t;

                if (auto mesure = items[1].is<CalcValue<PercentOr<Length>>>()) {
                    vValue = *mesure;
                } else {
                    secondPairIndex = 1;
                }

                return Ok();
            },
            [](auto&) -> Res<> {
                return Error::invalidData("invalid anchor");
            },
        }));

        try$(items[secondPairIndex].visit(Visitor{
            [&](Meta::Contains<Keywords::Left, Keywords::Right> auto& t) -> Res<> {
                if (hSet) {
                    if (hAnchor.is<Keywords::Center>()) { // the first center was aimed at the vertical part so we exchange
                        vAnchor = Keywords::CENTER;
                        vValue = hValue;
                    } else {
                        return Error::invalidData("expected vertical anchor");
                    }
                }

                hAnchor = t;

                if (secondPairIndex + 1 < items.len()) {
                    if (auto mesure = items[secondPairIndex + 1].is<CalcValue<PercentOr<Length>>>())
                        hValue = *mesure;
                    else {
                        return Error::invalidData("expected percent or length");
                    }
                }

                return Ok();
            },
            [&](Meta::Contains<Keywords::Top, Keywords::Bottom, Keywords::Center> auto& t) -> Res<> {
                vAnchor = t;

                if (secondPairIndex + 1 < items.len()) {
                    if (auto mesure = items[secondPairIndex + 1].is<CalcValue<PercentOr<Length>>>())
                        vValue = *mesure;
                    else {
                        return Error::invalidData("unexpected last value");
                    }
                }

                return Ok();
            },
            [](auto&) -> Res<> {
                return Error::invalidData("invalid anchor");
            },
        }));

        return Ok(BackgroundPosition(hAnchor, hValue, vAnchor, vValue));
    }
};

export struct BackgroundRepeat {
    enum _Val : u8 {
        NO = 0,
        X = 1 << 0,
        Y = 1 << 1,
        REPEAT = X | Y,
    };

    u8 _val;

    constexpr BackgroundRepeat(_Val val = REPEAT)
        : _val(static_cast<u8>(val)) {
    }

    void repr(Io::Emit& e) const {
        if (_val == NO) {
            e("no-repeat");
        } else if (_val == REPEAT) {
            e("repeat");
        } else {
            if (_val & X)
                e("repeat-x");
            if (_val & Y)
                e("repeat-y");
        }
    }
};

export struct BackgroundLayer {
    Opt<Image> image;
    BackgroundAttachment attachment;
    BackgroundPosition position;
    BackgroundRepeat repeat;

    void repr(Io::Emit& e) const {
        e("(background");
        e(" image={}", image);
        e(" attachment={}", attachment);
        e(" position={}", position);
        e(" repeat={}", repeat);
        e(")");
    }
};

export struct BackgroundProps {
    Color color = TRANSPARENT;
    Vec<BackgroundLayer> layers = {};

    void repr(Io::Emit& e) const {
        e("(background");
        e(" color={}", color);
        e(" layers={}", layers);
        e(")");
    }
};

} // namespace Vaev
