export module Karm.Core:base.niche;

import :meta.traits;
import :base.float_;

namespace Karm {

export template <typename T>
struct Niche;

export template <typename T>
concept Nicheable = requires {
    typename Niche<T>::Content;
};

export template <>
struct Niche<bool> {
    struct Content {
        u8 data;

        constexpr Content() : data(2) {}

        constexpr bool has() const {
            return data != 2;
        }
    };
};

export template <Meta::Enum T>
    requires requires { T::_LEN; }
struct Niche<T> {
    struct Content {
        using Underlying = Meta::UnderlyingType<T>;
        Underlying data;

        constexpr Content()
            : data(Underlying(T::_LEN) + 1) {}

        constexpr bool has() const {
            return data != (Underlying(T::_LEN) + 1);
        }
    };
};

export template <typename T>
    requires requires { FloatBits<T>::mantissaBits; }
struct Niche<T> {
    struct Content {
        FloatBits<T> bits;

        static constexpr FloatBits<T> _none = {
            .sign = 1,
            .exponent = FloatBits<T>::exponentMax,
            .mantissa = static_cast<decltype(FloatBits<T>::mantissaMax)>(0b11) << (FloatBits<T>::mantissaBits - 2)
        };

        constexpr Content() : bits(_none) {}

        constexpr bool has() const {
            return not(bits.sign == _none.sign and bits.exponent == _none.exponent and bits.mantissa == _none.mantissa);
        }
    };
};

export char const* NICHE_PTR = reinterpret_cast<char const*>(0x1);

} // namespace Karm
