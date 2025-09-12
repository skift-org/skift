module;

#include <karm-math/insets.h>
#include <karm-math/vec.h>

export module Karm.Print:paper;

import Karm.Core;

namespace Karm::Print {

export constexpr f64 UNIT = (1 / 25.4) * 96.0;

export struct PaperStock {
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

    void repr(Io::Emit& e) const {
        e("(paper {} {}x{})", name, width, height);
    }
};

export struct PaperSeries {
    Str name;
    Slice<PaperStock const> stocks;
};

// MARK: ISO Series ------------------------------------------------------------

export constexpr PaperStock A0 = {"A0", 841.0 * UNIT, 1189.0 * UNIT};
export constexpr PaperStock A1 = {"A1", 594.0 * UNIT, 841.0 * UNIT};
export constexpr PaperStock A2 = {"A2", 420.0 * UNIT, 594.0 * UNIT};
export constexpr PaperStock A3 = {"A3", 297.0 * UNIT, 420.0 * UNIT};
export constexpr PaperStock A4 = {"A4", 210.0 * UNIT, 297.0 * UNIT};
export constexpr PaperStock A5 = {"A5", 148.0 * UNIT, 210.0 * UNIT};
export constexpr PaperStock A6 = {"A6", 105.0 * UNIT, 148.0 * UNIT};
export constexpr PaperStock A7 = {"A7", 74.0 * UNIT, 105.0 * UNIT};
export constexpr PaperStock A8 = {"A8", 52.0 * UNIT, 74.0 * UNIT};
export constexpr PaperStock A9 = {"A9", 37.0 * UNIT, 52.0 * UNIT};
export constexpr PaperStock A10 = {"A10", 26.0 * UNIT, 37.0 * UNIT};

export constexpr Array _A_SERIES = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10};
export constexpr PaperSeries A_SERIES = {"A Series", _A_SERIES};

export constexpr PaperStock B0 = {"B0", 1000.0 * UNIT, 1414.0 * UNIT};
export constexpr PaperStock B1 = {"B1", 707.0 * UNIT, 1000.0 * UNIT};
export constexpr PaperStock B2 = {"B2", 500.0 * UNIT, 707.0 * UNIT};
export constexpr PaperStock B3 = {"B3", 353.0 * UNIT, 500.0 * UNIT};
export constexpr PaperStock B4 = {"B4", 250.0 * UNIT, 353.0 * UNIT};
export constexpr PaperStock B5 = {"B5", 176.0 * UNIT, 250.0 * UNIT};
export constexpr PaperStock B6 = {"B6", 125.0 * UNIT, 176.0 * UNIT};
export constexpr PaperStock B7 = {"B7", 88.0 * UNIT, 125.0 * UNIT};
export constexpr PaperStock B8 = {"B8", 62.0 * UNIT, 88.0 * UNIT};
export constexpr PaperStock B9 = {"B9", 33.0 * UNIT, 62.0 * UNIT};
export constexpr PaperStock B10 = {"B10", 31.0 * UNIT, 44.0 * UNIT};

export constexpr Array _B_SERIES = {B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10};
export constexpr PaperSeries B_SERIES = {"B Series", _B_SERIES};

export constexpr PaperStock C0 = {"C0", 917.0 * UNIT, 1297.0 * UNIT};
export constexpr PaperStock C1 = {"C1", 648.0 * UNIT, 917.0 * UNIT};
export constexpr PaperStock C2 = {"C2", 458.0 * UNIT, 648.0 * UNIT};
export constexpr PaperStock C3 = {"C3", 324.0 * UNIT, 458.0 * UNIT};
export constexpr PaperStock C4 = {"C4", 229.0 * UNIT, 324.0 * UNIT};
export constexpr PaperStock C5 = {"C5", 162.0 * UNIT, 229.0 * UNIT};
export constexpr PaperStock C6 = {"C6", 114.0 * UNIT, 162.0 * UNIT};
export constexpr PaperStock C7 = {"C7", 81.0 * UNIT, 114.0 * UNIT};
export constexpr PaperStock C8 = {"C8", 57.0 * UNIT, 81.0 * UNIT};
export constexpr PaperStock C9 = {"C9", 40.0 * UNIT, 57.0 * UNIT};
export constexpr PaperStock C10 = {"C10", 28.0 * UNIT, 40.0 * UNIT};

export constexpr Array _C_SERIES = {C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10};
export constexpr PaperSeries C_SERIES = {"C Series", _C_SERIES};

// MARK: JIS B-Series: Japanese Variation of ISO B Series  -----------------------------

export constexpr PaperStock JIS_B0 = {"JIS-B0", 1030.0 * UNIT, 1456.0 * UNIT};
export constexpr PaperStock JIS_B1 = {"JIS-B1", 728.0 * UNIT, 1030.0 * UNIT};
export constexpr PaperStock JIS_B2 = {"JIS-B2", 515.0 * UNIT, 728.0 * UNIT};
export constexpr PaperStock JIS_B3 = {"JIS-B3", 364.0 * UNIT, 515.0 * UNIT};
export constexpr PaperStock JIS_B4 = {"JIS-B4", 257.0 * UNIT, 364.0 * UNIT};
export constexpr PaperStock JIS_B5 = {"JIS-B5", 182.0 * UNIT, 257.0 * UNIT};
export constexpr PaperStock JIS_B6 = {"JIS-B6", 128.0 * UNIT, 182.0 * UNIT};
export constexpr PaperStock JIS_B7 = {"JIS-B7", 91.0 * UNIT, 128.0 * UNIT};
export constexpr PaperStock JIS_B8 = {"JIS-B8", 64.0 * UNIT, 91.0 * UNIT};
export constexpr PaperStock JIS_B9 = {"JIS-B9", 45.0 * UNIT, 64.0 * UNIT};
export constexpr PaperStock JIS_B10 = {"JIS-B10", 32.0 * UNIT, 45.0 * UNIT};
export constexpr PaperStock JIS_B11 = {"JIS-B11", 22.0 * UNIT, 32.0 * UNIT};
export constexpr PaperStock JIS_B12 = {"JIS-B12", 16.0 * UNIT, 22.0 * UNIT};

export constexpr Array _JIS_B_SERIES = {JIS_B0, JIS_B1, JIS_B2, JIS_B3, JIS_B4, JIS_B5, JIS_B6, JIS_B7, JIS_B8, JIS_B9, JIS_B10, JIS_B11, JIS_B12};
export constexpr PaperSeries JIS_B_SERIES = {"JIS-B Series", _JIS_B_SERIES};

// MARK: US Series -------------------------------------------------------------

export constexpr PaperStock EXECUTIVE = {"Executive", 190.5 * UNIT, 254.0 * UNIT};
export constexpr PaperStock FOLIO = {"Folio", 210.0 * UNIT, 330.0 * UNIT};
export constexpr PaperStock LEGAL = {"Legal", 215.9 * UNIT, 355.6 * UNIT};
export constexpr PaperStock LETTER = {"Letter", 215.9 * UNIT, 279.4 * UNIT};
export constexpr PaperStock TABLOID = {"Tabloid", 279.4 * UNIT, 431.8 * UNIT};
export constexpr PaperStock LEDGER = {"Ledger", 431.8 * UNIT, 279.4 * UNIT};

export constexpr Array _US_SERIES = {EXECUTIVE, FOLIO, LEGAL, LETTER, TABLOID, LEDGER};
export constexpr PaperSeries US_SERIES = {"US Series", _US_SERIES};

// MARK: Envelope Series -------------------------------------------------------

export constexpr PaperStock C5E = {"C5E", 229.0 * UNIT, 162.0 * UNIT};
export constexpr PaperStock COMM10E = {"Comm10E", 105.0 * UNIT, 241.0 * UNIT};
export constexpr PaperStock DLE = {"DLE", 110.0 * UNIT, 220.0 * UNIT};

export constexpr Array _ENVELOPE_SERIES = {C5E, COMM10E, DLE};
export constexpr PaperSeries ENVELOPE_SERIES = {"Envelope Series", _ENVELOPE_SERIES};

// MARK: All Paper Stocks ------------------------------------------------------

export constexpr Array SERIES = {
    A_SERIES,
    B_SERIES,
    C_SERIES,
    JIS_B_SERIES,
    US_SERIES,
    ENVELOPE_SERIES,
};

export Res<PaperStock> findPaperStock(Str name) {
    for (auto const& series : SERIES) {
        for (auto const& stock : series.stocks) {
            if (eqCi(stock.name, name))
                return Ok(stock);
        }
    }
    return Error::invalidData("unknown paper stock");
}

// MARK: Print Settings --------------------------------------------------------

export struct Margins {
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

    Margins(_Named named)
        : named(named) {}

    Margins(Math::Insetsf custom)
        : named(_Named::CUSTOM), custom(custom) {}

    bool operator==(_Named named) const {
        return this->named == named;
    }

    void repr(Io::Emit& e) const {
        e("{}", named);
    }
};

export enum struct Orientation {
    PORTRAIT,
    LANDSCAPE,

    _LEN,
};

export struct Settings {
    PaperStock paper = Print::A4;
    Margins margins = Margins::DEFAULT;
    Orientation orientation = Orientation::PORTRAIT;

    f64 scale = 1.;
    bool headerFooter = true;
    bool backgroundGraphics = true;
};

} // namespace Karm::Print
