#pragma once

#include "string.h"

namespace Karm {

using _TimeVal = u64;

// MARK: TimeSpan and TimeStamp ------------------------------------------------

struct TimeSpan {
    _TimeVal _value; // microseconds (us) aka 1/1,000,000th of a second

    static constexpr TimeSpan zero() {
        return TimeSpan{0};
    }

    static constexpr TimeSpan infinite() {
        return TimeSpan{~0uz};
    }

    static constexpr TimeSpan fromUSecs(_TimeVal value) {
        return {value};
    }

    static constexpr auto fromMSecs(_TimeVal value) {
        return fromUSecs(value * 1000);
    }

    static constexpr auto fromSecs(_TimeVal value) {
        return fromMSecs(value * 1000);
    }

    static constexpr auto fromMinutes(_TimeVal value) {
        return fromSecs(value * 60);
    }

    static constexpr auto fromHours(_TimeVal value) {
        return fromMinutes(value * 60);
    }

    static constexpr auto fromDays(_TimeVal value) {
        return fromHours(value * 24);
    }

    static constexpr auto fromWeeks(_TimeVal value) {
        return fromDays(value * 7);
    }

    static constexpr auto fromMonths(_TimeVal value) {
        return fromWeeks(value * 4);
    }

    static constexpr auto fromYears(_TimeVal value) {
        return fromMonths(value * 12);
    }

    constexpr TimeSpan()
        : _value(0) {}

    constexpr TimeSpan(_TimeVal value)
        : _value(value) {}

    constexpr bool isInfinite() const {
        return _value == ~0uz;
    }

    constexpr _TimeVal toUSecs() const {
        return _value;
    }

    constexpr _TimeVal toMSecs() const {
        return toUSecs() / 1000;
    }

    constexpr _TimeVal toSecs() const {
        return toMSecs() / 1000;
    }

    constexpr _TimeVal toMinutes() const {
        return toSecs() / 60;
    }

    constexpr _TimeVal toHours() const {
        return toMinutes() / 60;
    }

    constexpr _TimeVal toDays() const {
        return toHours() / 24;
    }

    constexpr _TimeVal toWeeks() const {
        return toDays() / 7;
    }

    constexpr _TimeVal toMonths() const {
        return toWeeks() / 4;
    }

    constexpr _TimeVal toYears() const {
        return toMonths() / 12;
    }

    constexpr TimeSpan &operator+=(TimeSpan other) {
        _value += other._value;
        return *this;
    }

    constexpr TimeSpan &operator-=(TimeSpan other) {
        _value -= other._value;
        return *this;
    }

    constexpr TimeSpan operator+(TimeSpan other) const {
        return _value + other._value;
    }

    constexpr TimeSpan operator-(TimeSpan other) const {
        return _value - other._value;
    }

    constexpr _TimeVal val() const {
        return _value;
    }

    auto operator<=>(TimeSpan const &other) const = default;
};

inline TimeSpan operator""_us(unsigned long long value) {
    return TimeSpan::fromUSecs(value);
}

inline TimeSpan operator""_ms(unsigned long long value) {
    return TimeSpan::fromMSecs(value);
}

inline TimeSpan operator""_s(unsigned long long value) {
    return TimeSpan::fromSecs(value);
}

inline TimeSpan operator""_m(unsigned long long value) {
    return TimeSpan::fromMinutes(value);
}

inline TimeSpan operator""_h(unsigned long long value) {
    return TimeSpan::fromHours(value);
}

inline TimeSpan operator""_d(unsigned long long value) {
    return TimeSpan::fromDays(value);
}

inline TimeSpan operator""_w(unsigned long long value) {
    return TimeSpan::fromWeeks(value);
}

inline TimeSpan operator""_M(unsigned long long value) {
    return TimeSpan::fromMonths(value);
}

inline TimeSpan operator""_y(unsigned long long value) {
    return TimeSpan::fromYears(value);
}

struct TimeStamp {
    _TimeVal _value{}; // microseconds (us) aka 1/1,000,000th of a second

    static constexpr _TimeVal END_OF_TIME = ~0uz;

    static constexpr TimeStamp epoch() {
        return {0};
    }

    static constexpr TimeStamp endOfTime() {
        return {END_OF_TIME};
    }

    constexpr TimeStamp(_TimeVal value = 0)
        : _value(value) {}

    constexpr bool isEndOfTime() const {
        return _value == END_OF_TIME;
    }

    constexpr TimeStamp &operator+=(TimeSpan other) {
        *this = *this + other;
        return *this;
    }

    constexpr TimeStamp &operator-=(TimeSpan other) {
        *this = *this - other;
        return *this;
    }

    constexpr TimeStamp operator+(TimeSpan other) const {
        if (other.isInfinite()) {
            return endOfTime();
        }
        if (isEndOfTime()) {
            return *this;
        }
        return _value + other._value;
    }

    constexpr TimeStamp operator-(TimeSpan other) const {
        if (isEndOfTime()) {
            return *this;
        }
        return _value - other._value;
    }

    constexpr TimeSpan operator-(TimeStamp other) const {
        if (isEndOfTime() or other.isEndOfTime()) {
            return TimeSpan::infinite();
        }
        return _value - other._value;
    }

    constexpr auto val() const {
        return _value;
    }

    auto operator<=>(TimeStamp const &) const = default;
};

// MARK: Time ------------------------------------------------------------------

struct Time {
    u8 second;
    u8 minute;
    u8 hour;

    static Time fromTimeSpan(TimeSpan span) {
        Time result;
        result.hour = span.toHours();
        span -= TimeSpan::fromHours(result.hour);
        result.minute = span.toMinutes();
        span -= TimeSpan::fromMinutes(result.minute);
        result.second = span.toSecs();
        return result;
    }

    TimeSpan toTimeSpan() const {
        return TimeSpan::fromSecs(second) +
               TimeSpan::fromMinutes(minute) +
               TimeSpan::fromHours(hour);
    }
};

struct Day {
    // NOTE: 0-based
    usize _raw;

    constexpr Day(usize raw = 0)
        : _raw(raw) {}

    constexpr operator usize() const {
        return _raw;
    }

    auto operator<=>(Day const &) const = default;
};

#define FOREACH_MONTH(MONTH) \
    MONTH(JANUARY, JAN)      \
    MONTH(FEBRUARY, FEB)     \
    MONTH(MARCH, MAR)        \
    MONTH(APRIL, APR)        \
    MONTH(MAY, MAY)          \
    MONTH(JUNE, JUN)         \
    MONTH(JULY, JUL)         \
    MONTH(AUGUST, AUG)       \
    MONTH(SEPTEMBER, SEP)    \
    MONTH(OCTOBER, OCT)      \
    MONTH(NOVEMBER, NOV)     \
    MONTH(DECEMBER, DEC)

struct Month {
    enum _Name {

#define ITER(NAME, ...) NAME,
        FOREACH_MONTH(ITER)
#undef ITER
    };

    // NOTE: Months are 0-based, so 0 is January
    usize _raw;

    constexpr Month(isize raw = 0)
        : _raw(raw) {}

    constexpr Month(_Name name)
        : _raw(name) {}

    constexpr operator usize() const {
        return _raw;
    }

    Month next() const {
        return (_raw + 1) % 12;
    }

    Str str() const {
        switch (_raw) {
#define ITER(NAME, ...) \
    case NAME:          \
        return #NAME;
            FOREACH_MONTH(ITER)
#undef ITER
        }
        panic("invalid month");
    }

    Str abbr() const {
        switch (_raw) {
#define ITER(NAME, ABBR) \
    case NAME:           \
        return #ABBR;
            FOREACH_MONTH(ITER)
#undef ITER
        }
        panic("invalid month");
    }

    usize val() const {
        return _raw;
    }

    auto operator<=>(Month const &) const = default;
};

struct Year {
    isize _raw;

    constexpr Year(isize raw = 0)
        : _raw(raw) {}

    constexpr bool isLeap() const {
        return (_raw % 4 == 0 and _raw % 100 != 0) or _raw % 400 == 0;
    }

    constexpr operator isize() const {
        return _raw;
    }

    Year next() const {
        return {_raw + 1};
    }

    Day days() const {
        return isLeap() ? 366 : 365;
    }

    constexpr Day daysIn(Month month) const {
        switch (month) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            return 31;
        case 3:
        case 5:
        case 8:
        case 10:
            return 30;
        case 1:
            return isLeap() ? 29 : 28;
        default:
            return 0;
        }
    }

    isize val() const {
        return _raw;
    }

    auto operator<=>(Year const &) const = default;
};

// MARK: Date ------------------------------------------------------------------

#define FOREACH_DAY_OF_WEEK(DOW) \
    DOW(MONDAY, MON)             \
    DOW(TUESDAY, TUE)            \
    DOW(WEDNESDAY, WED)          \
    DOW(THURSDAY, THU)           \
    DOW(FRIDAY, FRI)             \
    DOW(SATURDAY, SAT)           \
    DOW(SUNDAY, SUN)

struct DayOfWeek {
    enum _Raw : usize {

#define ITER(NAME, ...) NAME,
        FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
    };

    _Raw _raw;

    constexpr DayOfWeek(_Raw raw)
        : _raw(raw) {}

    constexpr DayOfWeek(usize raw)
        : _raw((_Raw)raw) {}

    constexpr operator usize() const {
        return _raw;
    }

    Str str() {
        switch (_raw) {
#define ITER(NAME, ...) \
    case NAME:          \
        return #NAME;
            FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
        default:
            panic("invalid day of week");
        }
    }

    Str abbr() {
        switch (_raw) {
#define ITER(NAME, ABBR) \
    case NAME:           \
        return #ABBR;
            FOREACH_DAY_OF_WEEK(ITER)
#undef ITER
        default:
            panic("invalid day of week");
        }
    }

    auto operator<=>(DayOfWeek const &) const = default;
};

struct Date {
    Day day;
    Month month;
    Year year;

    static Date epoch() {
        return Date{0, Month::JANUARY, 1970};
    }

    static Date fromTimeStamp(TimeStamp stamp) {
        auto span = stamp - TimeStamp::epoch();
        auto days = span.toDays();

        Year year{1970};
        while (days >= usize(year.days())) {
            days -= year.days();
            year = year.next();
        }

        Month month{0};
        while (days >= usize(year.daysIn(month))) {
            days -= year.daysIn(month);
            month = month.next();
        }

        return Date{days, month, year};
    }

    TimeStamp toTimeStamp() const {
        usize days{};

        for (isize y = 1970; y < isize(year); y++) {
            days += Year{y}.isLeap() ? 366 : 365;
        }

        for (isize m = 0; m < isize(month); m++) {
            days += year.daysIn(m);
        }

        days += day;

        return TimeStamp::epoch() + TimeSpan::fromDays(days);
    }

    /// Returns the day of the month, 0-based.
    u8 dayOfMonth() const {
        Date firstDay{0, month, year};
        return (toTimeStamp() - firstDay.toTimeStamp()).toDays();
    }

    DayOfWeek dayOfWeek() const {
        Date firstJan{0, 0, year};
        return DayOfWeek((toTimeStamp() - firstJan.toTimeStamp()).toDays() % 7);
    }

    usize dayOfYear() const {
        Date firstJan{0, 0, year};
        return (toTimeStamp() - firstJan.toTimeStamp()).toDays();
    }

    auto operator<=>(Date const &) const = default;
};

// MARK: DateTime --------------------------------------------------------------

struct DateTime {
    Date date;
    Time time;

    static DateTime epoch() {
        return DateTime{Date::epoch(), {}};
    }

    static DateTime fromTimeStamp(TimeStamp stamp) {
        Date date = Date::fromTimeStamp(stamp);
        Time time = Time::fromTimeSpan(stamp - date.toTimeStamp());
        return {date, time};
    }

    TimeStamp toTimeStamp() const {
        return date.toTimeStamp() + time.toTimeSpan();
    }
};

} // namespace Karm
