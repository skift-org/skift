export module Vaev.Engine:values.media;

import Karm.Core;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// MARK: Media Types -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#media-types

export enum struct MediaType {
    ALL,
    PRINT,
    SCREEN,
    OTHER, // tty, tv, projection, handheld, braille, aural, embossed

    _LEN,
};

export template <>
struct ValueParser<MediaType> {
    static Res<MediaType> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("all")))
            return Ok(MediaType::ALL);

        if (c.skip(Css::Token::ident("print")))
            return Ok(MediaType::PRINT);

        if (c.skip(Css::Token::ident("screen")))
            return Ok(MediaType::SCREEN);

        // NOTE: The spec says that we should reconize the following media types
        //       but they should not match anything
        static constexpr Array OTHER = {
            "tty",
            "tv",
            "projection",
            "handheld",
            "braille",
            "embossed",
            "speech",
            "aural",
        };

        for (auto const& item : OTHER) {
            if (c.skip(Css::Token::ident(item)))
                return Ok(MediaType::OTHER);
        }
        return Error::invalidData("expected media type");
    }
};

// MARK: Scan ------------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#scan

export enum struct Scan {
    INTERLACE,
    PROGRESSIVE,

    _LEN,
};

export template <>
struct ValueParser<Scan> {
    static Res<Scan> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("interlace")))
            return Ok(Scan::INTERLACE);
        else if (c.skip(Css::Token::ident("progressive")))
            return Ok(Scan::PROGRESSIVE);
        else
            return Error::invalidData("expected scan value");
    }
};

// MARK: Update ----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#update

export enum struct Update {
    NONE,
    SLOW,
    FAST,

    _LEN,
};

export template <>
struct ValueParser<Update> {
    static Res<Update> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Update::NONE);
        else if (c.skip(Css::Token::ident("slow")))
            return Ok(Update::SLOW);
        else if (c.skip(Css::Token::ident("fast")))
            return Ok(Update::FAST);
        else
            return Error::invalidData("expected update value");
    }
};

// MARK: OverflowBlock ---------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#mf-overflow-block

export enum struct OverflowBlock {
    NONE,
    SCROLL,
    PAGED,

    _LEN,
};

export template <>
struct ValueParser<OverflowBlock> {
    static Res<OverflowBlock> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(OverflowBlock::NONE);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(OverflowBlock::SCROLL);
        else if (c.skip(Css::Token::ident("paged")))
            return Ok(OverflowBlock::PAGED);
        else
            return Error::invalidData("expected overflow block value");
    }
};

// MARK: OverflowInline --------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline

export enum struct OverflowInline {
    NONE,
    SCROLL,

    _LEN,
};

export template <>
struct ValueParser<OverflowInline> {
    static Res<OverflowInline> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(OverflowInline::NONE);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(OverflowInline::SCROLL);
        else
            return Error::invalidData("expected overflow inline value");
    }
};

// MARK: Color Gamut -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#color-gamut

export enum struct ColorGamut {
    SRGB,
    DISPLAY_P3,
    REC2020,

    _LEN,
};

export template <>
struct ValueParser<ColorGamut> {
    static Res<ColorGamut> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("srgb")))
            return Ok(ColorGamut::SRGB);
        else if (c.skip(Css::Token::ident("p3")))
            return Ok(ColorGamut::DISPLAY_P3);
        else if (c.skip(Css::Token::ident("rec2020")))
            return Ok(ColorGamut::REC2020);
        else
            return Error::invalidData("expected color gamut");
    }
};

// MARK: Pointer ---------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#pointer

export enum struct Pointer {
    NONE,
    COARSE,
    FINE,

    _LEN,
};

export template <>
struct ValueParser<Pointer> {
    static Res<Pointer> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Pointer::NONE);
        else if (c.skip(Css::Token::ident("coarse")))
            return Ok(Pointer::COARSE);
        else if (c.skip(Css::Token::ident("fine")))
            return Ok(Pointer::FINE);
        else
            return Error::invalidData("expected pointer value");
    }
};

// MARK: Hover -----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#hover

export enum struct Hover {
    NONE,
    HOVER,

    _LEN,
};

export template <>
struct ValueParser<Hover> {
    static Res<Hover> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Hover::NONE);
        else if (c.skip(Css::Token::ident("hover")))
            return Ok(Hover::HOVER);
        else
            return Error::invalidData("expected hover value");
    }
};

// MARK: Reduce Motion ---------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#reduced-motion

export enum struct ReducedMotion {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

export template <>
struct ValueParser<ReducedMotion> {
    static Res<ReducedMotion> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedMotion::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedMotion::REDUCE);
        else
            return Error::invalidData("expected reduced motion value");
    }
};

// MARK: ReducedTransparency ---------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#reduced-transparency

export enum struct ReducedTransparency {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

export template <>
struct ValueParser<ReducedTransparency> {
    static Res<ReducedTransparency> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedTransparency::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedTransparency::REDUCE);
        else
            return Error::invalidData("expected reduced transparency value");
    }
};

// MARK: Contrast --------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#contrast

export enum struct Contrast {
    NO_PREFERENCE,
    MORE,
    LESS,

    _LEN,
};

export template <>
struct ValueParser<Contrast> {
    static Res<Contrast> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("less")))
            return Ok(Contrast::LESS);
        else if (c.skip(Css::Token::ident("more")))
            return Ok(Contrast::MORE);
        else
            return Error::invalidData("expected contrast value");
    }
};

// MARK: Colors ----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#forced-colors

export enum struct Colors {
    NONE,
    ACTIVE,

    _LEN,
};

export template <>
struct ValueParser<Colors> {
    static Res<Colors> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Colors::NONE);
        else if (c.skip(Css::Token::ident("active")))
            return Ok(Colors::ACTIVE);
        else
            return Error::invalidData("expected colors value");
    }
};

// MARK: Color Scheme ----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#color-scheme

export enum struct ColorScheme {
    LIGHT,
    DARK,

    _LEN,
};

export template <>
struct ValueParser<ColorScheme> {
    static Res<ColorScheme> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("light")))
            return Ok(ColorScheme::LIGHT);
        else if (c.skip(Css::Token::ident("dark")))
            return Ok(ColorScheme::DARK);
        else
            return Error::invalidData("expected color scheme value");
    }
};

// MARK: ReducedData
// https://drafts.csswg.org/mediaqueries/#reduced-data

export enum struct ReducedData {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

export template <>
struct ValueParser<ReducedData> {
    static Res<ReducedData> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedData::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedData::REDUCE);
        else
            return Error::invalidData("expected reduced data value");
    }
};

} // namespace Vaev
