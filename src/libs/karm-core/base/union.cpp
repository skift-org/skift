module;

#include <karm-core/macros.h>

export module Karm.Core:base.union_;

import :base.clamp;
import :base.cursor;
import :base.hash;
import :base.opt;
import :meta.pack;

namespace Karm {

export template <typename... Ts>
struct Union {
    static_assert(sizeof...(Ts) <= 255, "Union can only hold up to 255 types");

    alignas(max(alignof(Ts)...)) char _buf[max(Meta::zeroableSizeOf<Ts>()...)];
    u8 _index;

    always_inline Union()
        requires(Meta::Contains<None, Ts...>)
        : Union(None{}) {}

    template <Meta::Contains<Ts...> T>
    always_inline Union(T const& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union(T&& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(std::move(value));
    }

    always_inline Union(Union const& other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(
            _index, other._buf,
            [this]<typename T>(T const& ptr) {
                new (_buf) T(ptr);
            }
        );
    }

    always_inline Union(Union&& other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(std::move(ptr));
        });
    }

    always_inline ~Union() {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) {
            ptr.~T();
        });
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union& operator=(T const& value) {
        *this = Union(value);
        return *this;
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union& operator=(T&& value) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U& ptr) {
            ptr.~U();
        });

        _index = Meta::indexOf<T, Ts...>();
        new (_buf) T(std::move(value));

        return *this;
    }

    always_inline Union& operator=(Union const& other) {
        *this = Union(other);
        return *this;
    }

    always_inline Union& operator=(Union&& other) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) {
            ptr.~T();
        });

        _index = other._index;

        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            new (_buf) T(std::move(ptr));
        });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
    always_inline T& unwrap(char const* msg = "unwrapping wrong type") lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    always_inline T const& unwrap(char const* msg = "unwrapping wrong type") const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T const*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    always_inline T const& unwrapOr(T const& fallback) const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return fallback;

        return *reinterpret_cast<T const*>(_buf);
    }

    template <typename T, typename... Args>
    always_inline T& emplace(Args&&... args) {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U& ptr) {
                ptr.~U();
            });

            _index = Meta::indexOf<T, Ts...>();
        }

        return *new (_buf) T(std::forward<Args>(args)...);
    }

    template <Meta::Contains<Ts...> T>
    always_inline T take(char const* msg = "taking wrong type") {
        if (_index != Meta::indexOf<T, Ts...>())
            panic(msg);
        return std::move(*reinterpret_cast<T*>(_buf));
    }

    always_inline auto visit(auto visitor) {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline auto visit(auto visitor) const {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline static auto any(auto visitor) {
        return Meta::any<Ts...>(visitor);
    }

    template <Meta::Contains<Ts...> T>
    always_inline MutCursor<T> is() lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return nullptr;
        return (T*)_buf;
    }

    template <Meta::Contains<Ts...> T>
    always_inline Cursor<T> is() const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return nullptr;
        return (T const*)_buf;
    }

    always_inline usize index() const { return _index; }

    always_inline bool valid() const { return _index < sizeof...(Ts); }

    template <Meta::Contains<Ts...> T>
    std::partial_ordering operator<=>(T const& other) const {
        if constexpr (Meta::Comparable<T>)
            if (is<T>())
                return unwrap<T>() <=> other;
        return std::partial_ordering::unordered;
    }

    template <Meta::Contains<Ts...> T>
        requires Meta::Equatable<T>
    bool operator==(T const& other) const {
        if (is<T>())
            return unwrap<T>() == other;
        return false;
    }

    std::partial_ordering operator<=>(Union const& other) const {
        if (_index == other._index)
            return visit(
                [&]<typename T>(T const& ptr) {
                    if constexpr (Meta::Comparable<T>)
                        return ptr <=> other.unwrap<T>();
                    return std::partial_ordering::unordered;
                }
            );
        return std::partial_ordering::unordered;
    }

    bool operator==(Union const& other) const {
        if (_index == other._index)
            return visit(
                [&]<typename T>(T const& ptr) {
                    return ptr == other.unwrap<T>();
                }
            );
        return false;
    }

    u64 hash() const {
        return hash(
            hash(_index),
            visit([](auto const& v) {
                return Karm::hash(v);
            })
        );
    }
};

export template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

export template <typename... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

template <typename T, typename... Ts>
struct _FlattenUnion {
    using Type = T;
};

// If the type is already in the Union we discard it
template <typename... Ts, Meta::Contains<Ts...> A, typename... Us>
struct _FlattenUnion<Union<Ts...>, A, Us...> {
    using Type = _FlattenUnion<Union<Ts...>, Us...>::Type;
};

// Else we add it to the Union
template <typename... Ts, typename A, typename... Us>
struct _FlattenUnion<Union<Ts...>, A, Us...> {
    using Type = _FlattenUnion<Union<Ts..., A>, Us...>::Type;
};

template <typename... Ts, typename... Us, typename... Vs>
struct _FlattenUnion<Union<Ts...>, Union<Us...>, Vs...> {
    using Type = _FlattenUnion<Union<Ts...>, Us..., Vs...>::Type;
};

export template <typename... Ts>
using FlatUnion = _FlattenUnion<Union<>, Ts...>::Type;

export template <typename... Ts>
    requires(sizeof...(Ts) <= 254)
struct Niche<Union<Ts...>> {
    struct Content {
        alignas(max(alignof(Ts)...)) char _buf[max(Meta::zeroableSizeOf<Ts>()...)];
        u8 index;

        always_inline constexpr Content() : index(255) {}

        always_inline constexpr bool has() const {
            return index != 255;
        }
    };
};

} // namespace Karm
