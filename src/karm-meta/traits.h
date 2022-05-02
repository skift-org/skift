#pragma once

namespace Karm::Meta {

template <typename T>
concept Trivial = __is_trivial(T);

template <typename Derived, typename Base>
concept Derive = __is_base_of(Base, Derived);

} // namespace Karm::Meta
