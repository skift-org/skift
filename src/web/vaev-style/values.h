#pragma once

#include <vaev-base/align.h>
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
