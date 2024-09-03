#pragma once

#include <karm-base/distinct.h>
#include <karm-base/string.h>

namespace Karm::Print {

static constexpr f64 INCH_TO_MM = 25.4;

// Print density in Dot Per Mm (DPMM)
struct Density : public Distinct<f64, struct _DensityTag> {
    using Distinct::Distinct;

    static Density const DEFAULT;

    static constexpr Density fromDpi(f64 dpi) {
        return Density{dpi * INCH_TO_MM};
    }

    static constexpr Density fromDpcm(f64 dpcm) {
        return Density{dpcm};
    }

    constexpr f64 toDpi() const {
        return _value / INCH_TO_MM;
    }

    constexpr f64 toDpcm() const {
        return _value;
    }
};

inline constexpr Density Density::DEFAULT = Density::fromDpi(72.0);

struct PaperStock {
    Str name;
    f64 width;  // in mm
    f64 height; // in mm
};

// MARK: ISO Series ------------------------------------------------------------

static constexpr PaperStock A0 = {"A0", 841.0, 1189.0};
static constexpr PaperStock A1 = {"A1", 594.0, 841.0};
static constexpr PaperStock A2 = {"A2", 420.0, 594.0};
static constexpr PaperStock A3 = {"A3", 297.0, 420.0};
static constexpr PaperStock A4 = {"A4", 210.0, 297.0};
static constexpr PaperStock A5 = {"A5", 148.0, 210.0};
static constexpr PaperStock A6 = {"A6", 105.0, 148.0};
static constexpr PaperStock A7 = {"A7", 74.0, 105.0};
static constexpr PaperStock A8 = {"A8", 52.0, 74.0};
static constexpr PaperStock A9 = {"A9", 37.0, 52.0};
static constexpr PaperStock A10 = {"A10", 26.0, 37.0};

static constexpr Array A_SERIES = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10};

static constexpr PaperStock B0 = {"B0", 1000.0, 1414.0};
static constexpr PaperStock B1 = {"B1", 707.0, 1000.0};
static constexpr PaperStock B2 = {"B2", 500.0, 707.0};
static constexpr PaperStock B3 = {"B3", 353.0, 500.0};
static constexpr PaperStock B4 = {"B4", 250.0, 353.0};
static constexpr PaperStock B5 = {"B5", 176.0, 250.0};
static constexpr PaperStock B6 = {"B6", 125.0, 176.0};
static constexpr PaperStock B7 = {"B7", 88.0, 125.0};
static constexpr PaperStock B8 = {"B8", 62.0, 88.0};
static constexpr PaperStock B9 = {"B9", 33.0, 62.0};
static constexpr PaperStock B10 = {"B10", 31.0, 44.0};

static constexpr Array B_SERIES = {B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10};

static constexpr PaperStock C0 = {"C0", 917.0, 1297.0};
static constexpr PaperStock C1 = {"C1", 648.0, 917.0};
static constexpr PaperStock C2 = {"C2", 458.0, 648.0};
static constexpr PaperStock C3 = {"C3", 324.0, 458.0};
static constexpr PaperStock C4 = {"C4", 229.0, 324.0};
static constexpr PaperStock C5 = {"C5", 162.0, 229.0};
static constexpr PaperStock C6 = {"C6", 114.0, 162.0};
static constexpr PaperStock C7 = {"C7", 81.0, 114.0};
static constexpr PaperStock C8 = {"C8", 57.0, 81.0};
static constexpr PaperStock C9 = {"C9", 40.0, 57.0};
static constexpr PaperStock C10 = {"C10", 28.0, 40.0};

static constexpr Array C_SERIES = {C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10};

// MARK: US Series -------------------------------------------------------------

static constexpr PaperStock EXECUTIVE = {"Executive", 190.5, 254.0};
static constexpr PaperStock FOLIO = {"Folio", 210.0, 330.0};
static constexpr PaperStock LEGAL = {"Legal", 215.9, 355.6};
static constexpr PaperStock LETTER = {"Letter", 215.9, 279.4};
static constexpr PaperStock TABLOID = {"Tabloid", 279.4, 431.8};
static constexpr PaperStock LEDGER = {"Ledger", 431.8, 279.4};

static constexpr Array US_SERIES = {EXECUTIVE, FOLIO, LEGAL, LETTER, TABLOID, LEDGER};

// MARK: Envelope Series -------------------------------------------------------

static constexpr PaperStock C5E = {"C5E", 229.0, 162.0};
static constexpr PaperStock COMM10E = {"Comm10E", 105.0, 241.0};
static constexpr PaperStock DLE = {"DLE", 110.0, 220.0};

static constexpr Array ENVELOPE_SERIES = {C5E, COMM10E, DLE};

// MARK: All Paper Stocks ------------------------------------------------------

static constexpr Array PAPER_STOCKS = {
    sub(A_SERIES),
    B_SERIES,
    C_SERIES,
    US_SERIES,
    ENVELOPE_SERIES,
};

} // namespace Karm::Print
