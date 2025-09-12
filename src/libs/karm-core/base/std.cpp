module;

#include <compare>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

export module Karm.Core:base.std;

export namespace std {

// <compare>
using std::partial_ordering;
using std::strong_ordering;
using std::weak_ordering;

// <coroutine>
using std::coroutine_handle;
using std::coroutine_traits;
using std::suspend_always;
using std::suspend_never;

// <cstddef>
using std::nullptr_t;
using std::size_t;

// <cstdint>
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;

// <memory>
using std::addressof;
using std::construct_at;
using std::destroy_at;

// <cstring>
using std::memcpy;
using std::memset;
using std::strcmp;
using std::strlen;

// <initializer_list>
using std::initializer_list;

// <type_traits>
using std::integral_constant;

// <utility>
using std::declval;
using std::exchange;
using std::forward;
using std::index_sequence;
using std::integer_sequence;
using std::make_index_sequence;
using std::make_integer_sequence;
using std::move;
using std::swap;

} // namespace std

export using ::operator new;
