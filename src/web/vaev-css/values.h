#pragma once

#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/insets.h>
#include <vaev-base/length.h>
#include <vaev-base/media.h>
#include <vaev-base/numbers.h>
#include <vaev-base/resolution.h>
#include <vaev-base/sizing.h>

#include "parser.h"

namespace Vaev::Css {

// NOTE: Please keep this alphabetically sorted.

template <typename T>
struct ValueParser;

template <typename T>
always_inline static Res<T> parseValue(Cursor<Sst> &c) {
    return ValueParser<T>::parse(c);
}

template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Color> {
    static Res<Color> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<ColorGamut> {
    static Res<ColorGamut> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Display> {
    static Res<Display> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Hover> {
    static Res<Hover> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Length> {
    static Res<Length> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<MarginWidth> {
    static Res<MarginWidth> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<MediaType> {
    static Res<MediaType> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Number> {
    static Res<Number> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Orientation> {
    static Res<Orientation> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<OverflowBlock> {
    static Res<OverflowBlock> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<OverflowInline> {
    static Res<OverflowInline> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Percent> {
    static Res<Percent> parse(Cursor<Sst> &c);
};

template <typename T>
struct ValueParser<PercentOr<T>> {
    static Res<PercentOr<T>> parse(Cursor<Sst> &c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::PERCENTAGE)
            return Ok(try$(parseValue<Percent>(c)));

        return Ok(try$(parseValue<T>(c)));
    }
};

template <>
struct ValueParser<Pointer> {
    static Res<Pointer> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Resolution> {
    static Res<Resolution> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Scan> {
    static Res<Scan> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Size> {
    static Res<Size> parse(Cursor<Sst> &c);
};

template <>
struct ValueParser<Update> {
    static Res<Update> parse(Cursor<Sst> &c);
};

} // namespace Vaev::Css
