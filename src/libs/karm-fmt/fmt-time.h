#pragma once

#include <karm-base/time.h>

#include "base.h"

namespace Karm::Fmt {

template <>
struct Karm::Fmt::Formatter<Time> {
    Res<usize> format(Io::TextWriter &writer, Time time) {
        return Fmt::format(writer, "{02}:{02}:{02}", time.hour, time.minute, time.second);
    }
};

template <>
struct Karm::Fmt::Formatter<Date> {
    Res<usize> format(Io::TextWriter &writer, Date date) {
        return Fmt::format(writer, "{04}-{02}-{02}", (isize)date.year, (usize)date.month + 1, (usize)date.day + 1);
    }
};

template <>
struct Karm::Fmt::Formatter<DateTime> {
    Res<usize> format(Io::TextWriter &writer, DateTime dateTime) {
        return Fmt::format(writer, "{} {}", dateTime.date, dateTime.time);
    }
};

} // namespace Karm::Fmt
