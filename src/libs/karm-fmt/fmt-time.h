#pragma once

#include <karm-base/time.h>

#include "base.h"

namespace Karm::Fmt {

template <>
struct Karm::Fmt::Formatter<Time> {
    Res<usize> format(Io::TextWriter &writer, Time const &val) {
        return Fmt::format(writer, "{02}:{02}:{02}", val.hour, val.minute, val.second);
    }
};

template <>
struct Karm::Fmt::Formatter<Date> {
    Res<usize> format(Io::TextWriter &writer, Date const &val) {
        return Fmt::format(writer, "{04}-{02}-{02}", (isize)val.year, (usize)val.month + 1, (usize)val.day + 1);
    }
};

template <>
struct Karm::Fmt::Formatter<DateTime> {
    Res<usize> format(Io::TextWriter &writer, DateTime const &val) {
        return Fmt::format(writer, "{} {}", val.date, val.time);
    }
};

} // namespace Karm::Fmt
