#pragma once

#include "ordr.h"
#include "std.h"

namespace Karm {

using Tick = u64;

static constexpr Tick TICKS_PER_SECOND = 1000;

struct TimeSpan {
    u64 _value; // us

    static constexpr TimeSpan zero() {
        return TimeSpan{0};
    }

    static constexpr TimeSpan infinite() {
        return TimeSpan{~0ull};
    }

    static constexpr TimeSpan fromUSecs(u64 value) {
        return {value};
    }

    static constexpr auto fromMSecs(u64 value) {
        return fromUSecs(value * 1000);
    }

    static constexpr auto fromSecs(u64 value) {
        return fromMSecs(value * 1000);
    }

    static constexpr auto fromMinutes(u64 value) {
        return fromSecs(value * 60);
    }

    static constexpr auto fromHours(u64 value) {
        return fromMinutes(value * 60);
    }

    static constexpr auto fromDays(u64 value) {
        return fromHours(value * 24);
    }

    static constexpr auto fromWeeks(u64 value) {
        return fromDays(value * 7);
    }

    static constexpr auto fromMonths(u64 value) {
        return fromWeeks(value * 4);
    }

    static constexpr auto fromYears(u64 value) {
        return fromMonths(value * 12);
    }

    constexpr TimeSpan()
        : _value(0) {}

    constexpr TimeSpan(u64 value)
        : _value(value) {}

    constexpr bool isInfinite() const {
        return _value == ~0ull;
    }

    constexpr u64 toUSecs() const {
        return _value;
    }

    constexpr u64 toMSecs() const {
        return toUSecs() / 1000;
    }

    constexpr u64 toSecs() const {
        return toMSecs() / 1000;
    }

    constexpr u64 toMinutes() const {
        return toSecs() / 60;
    }

    constexpr u64 toHours() const {
        return toMinutes() / 60;
    }

    constexpr u64 toDays() const {
        return toHours() / 24;
    }

    constexpr u64 toWeeks() const {
        return toDays() / 7;
    }

    constexpr u64 toMonths() const {
        return toWeeks() / 4;
    }

    constexpr u64 toYears() const {
        return toMonths() / 12;
    }

    constexpr Tick toTicks() {
        return _value / (1000 * 1000 / TICKS_PER_SECOND);
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

    constexpr Ordr cmp(TimeSpan other) const {
        return ::Karm::cmp(_value, other._value);
    }
};

struct TimeStamp {
    u64 _value;

    static constexpr u64 END_OF_TIME = {~0ull};

    static constexpr TimeStamp epoch() {
        return {0};
    }

    static constexpr TimeStamp endOfTime() {
        return {~0ull};
    }

    constexpr TimeStamp(u64 value = 0)
        : _value(value) {}

    constexpr bool isEndOfTime() const {
        return _value == ~0ull;
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
        if (isEndOfTime() || other.isEndOfTime()) {
            return TimeSpan::infinite();
        }
        return _value - other._value;
    }

    constexpr Ordr cmp(TimeStamp other) const {
        return ::Karm::cmp(_value, other._value);
    }
};

struct Time {
    u8 second;
    u8 minute;
    u8 hour;
};

struct Date {
    u8 day;
    u8 month;
    u16 year;
};

union DateTime {
    struct {
        Date date;
        Time time;
    };

    struct {
        u8 second;
        u8 minute;
        u8 hour;
        u8 day;
        u8 month;
        u16 year;
    };
};

} // namespace Karm
