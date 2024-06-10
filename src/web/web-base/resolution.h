#pragma once

#include <karm-io/fmt.h>
#include <karm-math/const.h>

namespace Web {

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

struct Resolution {
    enum struct Unit {
        DPI,
        DPCM,
        DPPX,
        INFINITE,
    };

    using enum Unit;

    f64 _val;
    Unit _unit;

    static Resolution fromDpi(f64 val) {
        return {val, Unit::DPI};
    }

    static Resolution fromDpcm(f64 val) {
        return {val, Unit::DPCM};
    }

    static Resolution fromDppx(f64 val) {
        return {val, Unit::DPPX};
    }

    static Resolution infinite() {
        return {0.0, Unit::INFINITE};
    }

    f64 val() const {
        return _val;
    }

    Unit unit() const {
        return _unit;
    }

    f64 toDpi() const {
        switch (_unit) {
        case Unit::DPI:
            return _val;
        case Unit::DPCM:
            return _val * 2.54;
        case Unit::DPPX:
            return _val * 96.0;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    f64 toDpcm() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 2.54;
        case Unit::DPCM:
            return _val;
        case Unit::DPPX:
            return _val * 96.0 / 2.54;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    f64 toDppx() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 96.0;
        case Unit::DPCM:
            return _val * 2.54 / 96.0;
        case Unit::DPPX:
            return _val;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    bool operator==(Resolution const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Resolution const &other) const {
        return toDpi() <=> other.toDpi();
    }
};

} // namespace Web

template <>
struct Karm::Io::Formatter<Web::Resolution> {
    Res<usize> format(Io::TextWriter &writer, Web::Resolution const &val) {
        usize written = try$(Io::format(writer, " {#}{#}", val.val(), val.unit()));
        return Ok(written);
    }
};
