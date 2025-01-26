#pragma once

#include "string.h"

namespace Karm {

using _TimeVal = u64;

// MARK: Duration and TimeStamp ------------------------------------------------

struct Duration {
    _TimeVal _value; // microseconds (us) aka 1/1,000,000th of a second

    static constexpr Duration zero() {
        return Duration{0};
    }

    static constexpr Duration infinite() {
        return Duration{~0uz};
    }

    static constexpr Duration fromUSecs(_TimeVal value) {
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

    constexpr Duration()
        : _value(0) {}

    constexpr Duration(_TimeVal value)
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

    constexpr Duration& operator+=(Duration other) {
        _value += other._value;
        return *this;
    }

    constexpr Duration& operator-=(Duration other) {
        _value -= other._value;
        return *this;
    }

    constexpr Duration operator+(Duration other) const {
        return _value + other._value;
    }

    constexpr Duration operator-(Duration other) const {
        return _value - other._value;
    }

    constexpr _TimeVal val() const {
        return _value;
    }

    auto operator<=>(Duration const& other) const = default;
    bool operator==(Duration const& other) const = default;
};

inline Duration operator""_us(unsigned long long value) {
    return Duration::fromUSecs(value);
}

inline Duration operator""_ms(unsigned long long value) {
    return Duration::fromMSecs(value);
}

inline Duration operator""_s(unsigned long long value) {
    return Duration::fromSecs(value);
}

inline Duration operator""_m(unsigned long long value) {
    return Duration::fromMinutes(value);
}

inline Duration operator""_h(unsigned long long value) {
    return Duration::fromHours(value);
}

inline Duration operator""_d(unsigned long long value) {
    return Duration::fromDays(value);
}

inline Duration operator""_w(unsigned long long value) {
    return Duration::fromWeeks(value);
}

inline Duration operator""_M(unsigned long long value) {
    return Duration::fromMonths(value);
}

inline Duration operator""_y(unsigned long long value) {
    return Duration::fromYears(value);
}

struct MonotonicClock {
    static constexpr bool monotonic = true;
};

struct SystemClock {
    static constexpr bool monotonic = false;
};

template <typename Clock>
struct _Instant {
    _TimeVal _value{}; // microseconds (us) aka 1/1,000,000th of a second

    static constexpr _TimeVal END_OF_TIME = ~0uz;

    static constexpr _Instant epoch() {
        return {0};
    }

    static constexpr _Instant endOfTime() {
        return {END_OF_TIME};
    }

    constexpr _Instant(_TimeVal value = 0)
        : _value(value) {}

    constexpr bool isEndOfTime() const {
        return _value == END_OF_TIME;
    }

    constexpr _Instant& operator+=(Duration other) {
        *this = *this + other;
        return *this;
    }

    constexpr _Instant& operator-=(Duration other) {
        *this = *this - other;
        return *this;
    }

    constexpr _Instant operator+(Duration other) const {
        if (other.isInfinite()) {
            return endOfTime();
        }
        if (isEndOfTime()) {
            return *this;
        }
        return _value + other._value;
    }

    constexpr _Instant operator-(Duration other) const {
        if (isEndOfTime()) {
            return *this;
        }
        return _value - other._value;
    }

    constexpr Duration operator-(_Instant other) const {
        if (isEndOfTime() or other.isEndOfTime()) {
            return Duration::infinite();
        }
        return _value - other._value;
    }

    constexpr auto val() const {
        return _value;
    }

    auto operator<=>(_Instant const&) const = default;
    bool operator==(_Instant const&) const = default;
};

using Instant = _Instant<MonotonicClock>;

using SystemTime = _Instant<SystemClock>;

// MARK: Time ------------------------------------------------------------------

struct Time {
    u8 second;
    u8 minute;
    u8 hour;

    static Time fromDuration(Duration span) {
        Time result;
        result.hour = span.toHours();
        span -= Duration::fromHours(result.hour);
        result.minute = span.toMinutes();
        span -= Duration::fromMinutes(result.minute);
        result.second = span.toSecs();
        return result;
    }

    Duration toDuration() const {
        return Duration::fromSecs(second) +
               Duration::fromMinutes(minute) +
               Duration::fromHours(hour);
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

    auto operator<=>(Day const&) const = default;
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

    auto operator<=>(Month const&) const = default;
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

    auto operator<=>(Year const&) const = default;
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

    auto operator<=>(DayOfWeek const&) const = default;
};

struct Date {
    Day day;
    Month month;
    Year year;

    static Date epoch() {
        return Date{0, Month::JANUARY, 1970};
    }

    static Date fromInstant(SystemTime stamp) {
        auto span = stamp - SystemTime::epoch();
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

    SystemTime toInstant() const {
        usize days{};

        for (isize y = 1970; y < isize(year); y++) {
            days += Year{y}.isLeap() ? 366 : 365;
        }

        for (isize m = 0; m < isize(month); m++) {
            days += year.daysIn(m);
        }

        days += day;

        return SystemTime::epoch() + Duration::fromDays(days);
    }

    /// Returns the day of the month, 0-based.
    u8 dayOfMonth() const {
        Date firstDay{0, month, year};
        return (toInstant() - firstDay.toInstant()).toDays();
    }

    DayOfWeek dayOfWeek() const {
        Date firstJan{0, 0, year};
        return DayOfWeek((toInstant() - firstJan.toInstant()).toDays() % 7);
    }

    usize dayOfYear() const {
        Date firstJan{0, 0, year};
        return (toInstant() - firstJan.toInstant()).toDays();
    }

    auto operator<=>(Date const&) const = default;
};

// MARK: DateTime --------------------------------------------------------------

struct DateTime {
    Date date;
    Time time;

    static DateTime epoch() {
        return DateTime{Date::epoch(), {}};
    }

    static DateTime fromInstant(SystemTime stamp) {
        Date date = Date::fromInstant(stamp);
        Time time = Time::fromDuration(stamp - date.toInstant());
        return {date, time};
    }

    SystemTime toInstant() const {
        return date.toInstant() + time.toDuration();
    }
};

} // namespace Karm
