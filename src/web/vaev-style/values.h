#pragma once

#include <vaev-base/align.h>
#include <vaev-base/borders.h>
#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/flex.h>
#include <vaev-base/font.h>
#include <vaev-base/insets.h>
#include <vaev-base/length.h>
#include <vaev-base/media.h>
#include <vaev-base/numbers.h>
#include <vaev-base/overflow.h>
#include <vaev-base/resolution.h>
#include <vaev-base/sizing.h>
#include <vaev-css/parser.h>

namespace Vaev::Style {

// MARK: Parser ----------------------------------------------------------------

// NOTE: Please keep this alphabetically sorted.

template <typename T>
struct ValueParser;

template <typename T>
always_inline static Res<T> parseValue(Cursor<Css::Sst> &c) {
    return ValueParser<T>::parse(c);
}

template <>
struct ValueParser<Align> {
    static Res<Align> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Angle> {
    static Res<Angle> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<BorderStyle> {
    static Res<BorderStyle> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Color> {
    static Res<Color> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<ColorGamut> {
    static Res<ColorGamut> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Display> {
    static Res<Display> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FlexDirection> {
    static Res<FlexDirection> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FlexWrap> {
    static Res<FlexWrap> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FlexBasis> {
    static Res<FlexBasis> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FontSize> {
    static Res<FontSize> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FontStyle> {
    static Res<FontStyle> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FontWeight> {
    static Res<FontWeight> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<FontWidth> {
    static Res<FontWidth> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Hover> {
    static Res<Hover> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Css::Sst> &c);
};

template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Css::Sst> &c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

template <>
struct ValueParser<Length> {
    static Res<Length> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Width> {
    static Res<Width> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<MediaType> {
    static Res<MediaType> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Number> {
    static Res<Number> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Orientation> {
    static Res<Orientation> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Overflow> {
    static Res<Overflow> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<OverflowBlock> {
    static Res<OverflowBlock> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<OverflowInline> {
    static Res<OverflowInline> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Percent> {
    static Res<Percent> parse(Cursor<Css::Sst> &c);
};

template <typename T>
struct ValueParser<PercentOr<T>> {
    static Res<PercentOr<T>> parse(Cursor<Css::Sst> &c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::PERCENTAGE)
            return Ok(try$(parseValue<Percent>(c)));

        return Ok(try$(parseValue<T>(c)));
    }
};

template <>
struct ValueParser<Pointer> {
    static Res<Pointer> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Position> {
    static Res<Position> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Resolution> {
    static Res<Resolution> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Scan> {
    static Res<Scan> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Size> {
    static Res<Size> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<String> {
    static Res<String> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Update> {
    static Res<Update> parse(Cursor<Css::Sst> &c);
};

template <typename T>
struct ValueParser<Math::Radii<T>> {

    static Res<Math::Radii<T>> parse(Cursor<Css::Sst> &c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<PercentOr<Length>>(c);
        if (not value1)
            return Ok(Math::Radii<T>{});

        auto value2 = parseValue<PercentOr<Length>>(c);
        if (not value2)
            return Ok(Math::Radii<T>{value1.take()});

        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3)
            return Ok(Math::Radii<T>{value1.take(), value2.take()});

        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4)
            return Ok(Math::Radii<T>{value1.take(), value2.take(), value3.take(), value2.take()});

        if (not c.ended() and c.peek().token.data == "/"s) {
            auto value1 = parseValue<PercentOr<Length>>(c);
            if (not value1)
                return Ok(Math::Radii<T>{});

            auto value2 = parseValue<PercentOr<Length>>(c);
            if (not value2)
                return Ok(Math::Radii<T>{value1.take()});

            auto value3 = parseValue<PercentOr<Length>>(c);
            if (not value3)
                return Ok(Math::Radii<T>{value1.take(), value2.take()});

            auto value4 = parseValue<PercentOr<Length>>(c);
            if (not value4)
                return Ok(Math::Radii<T>{value1.take(), value2.take(), value3.take(), value2.take()});

            // if parse a /
            // 1 value-- > border all(a, d, e, h)
            // 2 values-- > 1 = top - start + bottom - end 2 = the others
            // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
            // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - ens, 4 = bottom - start
        }

        return Ok(Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()});
    }
};

template <>
struct ValueParser<ReducedMotion> {
    static Res<ReducedMotion> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<ReducedTransparency> {
    static Res<ReducedTransparency> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Contrast> {
    static Res<Contrast> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<Colors> {
    static Res<Colors> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<ColorScheme> {
    static Res<ColorScheme> parse(Cursor<Css::Sst> &c);
};

template <>
struct ValueParser<ReducedData> {
    static Res<ReducedData> parse(Cursor<Css::Sst> &c);
};

} // namespace Vaev::Style
