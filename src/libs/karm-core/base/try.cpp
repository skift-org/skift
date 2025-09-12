export module Karm.Core:base.try_;

import :meta.cvrp;
import :base.base;

namespace Karm {

export template <typename T>
concept Takeable = requires(T t) {
    { t.take() };
};

// MARK: Unwrapable ------------------------------------------------------------

export template <typename T>
concept Unwrapable = requires(T t) {
    { t.unwrap() };
};

// MARK: Tryable ---------------------------------------------------------------

export template <typename T>
concept Tryable = requires(T t) {
    { not static_cast<bool>(t) };
    { t.none() };
} and Unwrapable<T> and Takeable<T>;

} // namespace Karm
