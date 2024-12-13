#pragma once

#include <karm-base/distinct.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-math/insets.h>
#include <karm-math/vec.h>

namespace Karm::Print {

static constexpr f64 UNIT = (1 / 25.4) * 96.0;

struct PaperStock {
    // Size in CSS pixels at 96 DPI
    Str name;
    f64 width;
    f64 height;

    Math::Vec2f size() const {
        return {width, height};
    }

    f64 aspect() const {
        return width / height;
    }

    PaperStock landscape() const {
        return {name, height, width};
    }
};

struct PaperSeries {
    Str name;
    Slice<PaperStock const> stocks;
};

// MARK: ISO Series ------------------------------------------------------------

static constexpr PaperStock A0 = {"A0", 841.0 * UNIT, 1189.0 * UNIT};
static constexpr PaperStock A1 = {"A1", 594.0 * UNIT, 841.0 * UNIT};
static constexpr PaperStock A2 = {"A2", 420.0 * UNIT, 594.0 * UNIT};
static constexpr PaperStock A3 = {"A3", 297.0 * UNIT, 420.0 * UNIT};
static constexpr PaperStock A4 = {"A4", 210.0 * UNIT, 297.0 * UNIT};
static constexpr PaperStock A5 = {"A5", 148.0 * UNIT, 210.0 * UNIT};
static constexpr PaperStock A6 = {"A6", 105.0 * UNIT, 148.0 * UNIT};
static constexpr PaperStock A7 = {"A7", 74.0 * UNIT, 105.0 * UNIT};
static constexpr PaperStock A8 = {"A8", 52.0 * UNIT, 74.0 * UNIT};
static constexpr PaperStock A9 = {"A9", 37.0 * UNIT, 52.0 * UNIT};
static constexpr PaperStock A10 = {"A10", 26.0 * UNIT, 37.0 * UNIT};

static constexpr Array _A_SERIES = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10};
static constexpr PaperSeries A_SERIES = {"A Series", _A_SERIES};

static constexpr PaperStock B0 = {"B0", 1000.0 * UNIT, 1414.0 * UNIT};
static constexpr PaperStock B1 = {"B1", 707.0 * UNIT, 1000.0 * UNIT};
static constexpr PaperStock B2 = {"B2", 500.0 * UNIT, 707.0 * UNIT};
static constexpr PaperStock B3 = {"B3", 353.0 * UNIT, 500.0 * UNIT};
static constexpr PaperStock B4 = {"B4", 250.0 * UNIT, 353.0 * UNIT};
static constexpr PaperStock B5 = {"B5", 176.0 * UNIT, 250.0 * UNIT};
static constexpr PaperStock B6 = {"B6", 125.0 * UNIT, 176.0 * UNIT};
static constexpr PaperStock B7 = {"B7", 88.0 * UNIT, 125.0 * UNIT};
static constexpr PaperStock B8 = {"B8", 62.0 * UNIT, 88.0 * UNIT};
static constexpr PaperStock B9 = {"B9", 33.0 * UNIT, 62.0 * UNIT};
static constexpr PaperStock B10 = {"B10", 31.0 * UNIT, 44.0 * UNIT};

static constexpr Array _B_SERIES = {B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10};
static constexpr PaperSeries B_SERIES = {"B Series", _B_SERIES};

static constexpr PaperStock C0 = {"C0", 917.0 * UNIT, 1297.0 * UNIT};
static constexpr PaperStock C1 = {"C1", 648.0 * UNIT, 917.0 * UNIT};
static constexpr PaperStock C2 = {"C2", 458.0 * UNIT, 648.0 * UNIT};
static constexpr PaperStock C3 = {"C3", 324.0 * UNIT, 458.0 * UNIT};
static constexpr PaperStock C4 = {"C4", 229.0 * UNIT, 324.0 * UNIT};
static constexpr PaperStock C5 = {"C5", 162.0 * UNIT, 229.0 * UNIT};
static constexpr PaperStock C6 = {"C6", 114.0 * UNIT, 162.0 * UNIT};
static constexpr PaperStock C7 = {"C7", 81.0 * UNIT, 114.0 * UNIT};
static constexpr PaperStock C8 = {"C8", 57.0 * UNIT, 81.0 * UNIT};
static constexpr PaperStock C9 = {"C9", 40.0 * UNIT, 57.0 * UNIT};
static constexpr PaperStock C10 = {"C10", 28.0 * UNIT, 40.0 * UNIT};

static constexpr Array _C_SERIES = {C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10};
static constexpr PaperSeries C_SERIES = {"C Series", _C_SERIES};

// MARK: US Series -------------------------------------------------------------

static constexpr PaperStock EXECUTIVE = {"Executive", 190.5 * UNIT, 254.0 * UNIT};
static constexpr PaperStock FOLIO = {"Folio", 210.0 * UNIT, 330.0 * UNIT};
static constexpr PaperStock LEGAL = {"Legal", 215.9 * UNIT, 355.6 * UNIT};
static constexpr PaperStock LETTER = {"Letter", 215.9 * UNIT, 279.4 * UNIT};
static constexpr PaperStock TABLOID = {"Tabloid", 279.4 * UNIT, 431.8 * UNIT};
static constexpr PaperStock LEDGER = {"Ledger", 431.8 * UNIT, 279.4 * UNIT};

static constexpr Array _US_SERIES = {EXECUTIVE, FOLIO, LEGAL, LETTER, TABLOID, LEDGER};
static constexpr PaperSeries US_SERIES = {"US Series", _US_SERIES};

// MARK: Envelope Series -------------------------------------------------------

static constexpr PaperStock C5E = {"C5E", 229.0 * UNIT, 162.0 * UNIT};
static constexpr PaperStock COMM10E = {"Comm10E", 105.0 * UNIT, 241.0 * UNIT};
static constexpr PaperStock DLE = {"DLE", 110.0 * UNIT, 220.0 * UNIT};

static constexpr Array _ENVELOPE_SERIES = {C5E, COMM10E, DLE};
static constexpr PaperSeries ENVELOPE_SERIES = {"Envelope Series", _ENVELOPE_SERIES};

// MARK: All Paper Stocks ------------------------------------------------------

static constexpr Array SERIES = {
    A_SERIES,
    B_SERIES,
    C_SERIES,
    US_SERIES,
    ENVELOPE_SERIES,
};

static inline Res<PaperStock> findPaperStock(Str name) {
    for (auto const &series : SERIES) {
        for (auto const &stock : series.stocks) {
            if (eqCi(stock.name, name))
                return Ok(stock);
        }
    }
    return Error::invalidData("unknown paper stock");
}

// MARK: Print Settings --------------------------------------------------------

struct Margins {
    enum struct _Named {
        DEFAULT,
        NONE,
        MINIMUM,
        CUSTOM,

        _LEN
    };
    using enum _Named;
    _Named named;
    Math::Insetsf custom = 20 * UNIT;

    Margins(_Named named) : named(named) {}

    Margins(Math::Insetsf custom) : named(_Named::CUSTOM), custom(custom) {}

    bool operator==(_Named named) const {
        return this->named == named;
    }

    void repr(Io::Emit &e) const {
        e("{}", named);
    }
};

enum struct Orientation {
    PORTRAIT,
    LANDSCAPE,

    _LEN,
};

struct Settings {
    PaperStock paper = Print::A4;
    Margins margins = Margins::DEFAULT;
    Orientation orientation = Orientation::PORTRAIT;

    double scale = 1.;
    bool headerFooter = true;
    bool backgroundGraphics = false;
};

} // namespace Karm::Print
