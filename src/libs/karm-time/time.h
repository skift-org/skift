#pragma once

#include <karm-base/std.h>

namespace Karm::Time {

struct Span {
    uint64_t value;

    static constexpr Span zero() {
        return Span{0};
    }

    static constexpr Span infinite() {
        return Span{~0ull};
    }

    static constexpr Span fromUSecs(uint64_t value) {
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

    constexpr Span() : value(0) {}

    constexpr Span(uint64_t value) : value(value) {}

    constexpr bool isInfinite() const {
        return value == ~0ull;
    }

    constexpr uint64_t toUSecs() const {
        return value;
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

    constexpr Span &operator+=(Span const &other) {
        value += other.value;
        return *this;
    }

    constexpr Span &operator-=(Span const &other) {
        value -= other.value;
        return *this;
    }

    constexpr Span operator+(Span const &other) const {
        return value + other.value;
    }

    constexpr Span operator-(Span const &other) const {
        return value - other.value;
    }
};

struct Stamp {
    uint64_t value;

    static constexpr Stamp epoch() {
        return {0};
    }

    static constexpr Stamp endOfTime() {
        return {~0ull};
    }

    constexpr Stamp(uint64_t value = 0) : value(value) {}

    constexpr bool isEndOfTime() const {
        return value == ~0ull;
    }

    constexpr Stamp &operator+=(Span const &other) {
        *this = *this + other;
        return *this;
    }

    constexpr Stamp &operator-=(Span const &other) {
        *this = *this - other;
        return *this;
    }

    constexpr Stamp operator+(Span const &other) const {
        if (other.isInfinite()) {
            return endOfTime();
        }
        if (isEndOfTime()) {
            return *this;
        }
        return value + other.value;
    }

    constexpr Stamp operator-(Span const &other) const {
        if (isEndOfTime()) {
            return *this;
        }
        return value - other.value;
    }
};

} // namespace Karm::Time
