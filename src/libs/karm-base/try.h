#pragma once

#include <karm-meta/cvrp.h>

#include "base.h"

namespace Karm {

template <typename T>
concept Takeable = requires(T t) {
    { t.take() };
};

// MARK: Unwrapable ------------------------------------------------------------

template <typename T>
concept Unwrapable = requires(T t) {
    { t.unwrap() };
};

// MARK: Tryable ---------------------------------------------------------------

template <typename T>
concept Tryable = requires(T t) {
    { not static_cast<bool>(t) };
    { t.none() };
} and Unwrapable<T> and Takeable<T>;

} // namespace Karm
