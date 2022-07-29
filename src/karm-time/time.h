#pragma once

#include <karm-base/std.h>

namespace Karm::Time {

struct Timespan {
    uint64_t _value;

    static constexpr Timespan zero() {
        return Timespan{0};
    }

    static constexpr Timespan fromUSecs(uint64_t value) {
        return {value};
    }

    static constexpr auto fromMSecs(uint64_t value) {
        return fromUSecs(value * 1000);
    }

    static constexpr auto fromSecs(uint64_t value) {
        return fromMSecs(value * 1000);
    }

    static constexpr auto fromMinutes(uint64_t value) {
        return fromSecs(value * 60);
    }

    static constexpr auto fromHours(uint64_t value) {
        return fromMinutes(value * 60);
    }

    static constexpr auto fromDays(uint64_t value) {
        return fromHours(value * 24);
    }

    static constexpr auto fromWeeks(uint64_t value) {
        return fromDays(value * 7);
    }

    static constexpr auto fromMonths(uint64_t value) {
        return fromWeeks(value * 4);
    }

    static constexpr auto fromYears(uint64_t value) {
        return fromMonths(value * 12);
    }

    uint64_t value() const {
        return _value;
    }

    constexpr uint64_t toUSecs() const {
        return _value;
    }

    constexpr uint64_t toMSecs() const {
        return toUSecs() / 1000;
    }

    constexpr uint64_t toSecs() const {
        return toMSecs() / 1000;
    }

    constexpr uint64_t toMinutes() const {
        return toSecs() / 60;
    }

    constexpr uint64_t toHours() const {
        return toMinutes() / 60;
    }

    constexpr uint64_t toDays() const {
        return toHours() / 24;
    }

    constexpr uint64_t toWeeks() const {
        return toDays() / 7;
    }

    constexpr uint64_t toMonths() const {
        return toWeeks() / 4;
    }

    constexpr uint64_t toYears() const {
        return toMonths() / 12;
    }

    constexpr Timespan &operator+=(Timespan const &other) {
        _value += other._value;
        return *this;
    }

    constexpr Timespan &operator-=(Timespan const &other) {
        _value -= other._value;
        return *this;
    }

    constexpr Timespan &operator*=(uint64_t value) {
        _value *= value;
        return *this;
    }

    constexpr Timespan &operator/=(uint64_t value) {
        _value /= value;
        return *this;
    }

    constexpr Timespan operator+(Timespan const &other) const {
        return Timespan{_value + other._value};
    }

    constexpr Timespan operator-(Timespan const &other) const {
        return Timespan{_value - other._value};
    }
};

struct Timestamp {
    uint64_t _value;
};

} // namespace Karm::Time
