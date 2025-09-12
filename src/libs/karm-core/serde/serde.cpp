module;

#include <karm-core/macros.h>

export module Karm.Core:serde.serde;

import :base.base;
import :base.res;
import :base.string;
import :base.vec;
import :base.symbol;
import :base.map;

namespace Karm::Serde {

export enum struct SizeHint {
    N8,
    N16,
    N32,
    N64,

    AUTO
};

export template <typename T>
    requires Meta::Integral<T> or Meta::Float<T>
SizeHint sizeHintFor() {
    if constexpr (sizeof(T) == 1)
        return SizeHint::N8;
    else if constexpr (sizeof(T) == 2)
        return SizeHint::N16;
    else if constexpr (sizeof(T) == 4)
        return SizeHint::N32;
    else if constexpr (sizeof(T) == 8)
        return SizeHint::N64;
    else
        static_assert(false, "unable to infer size hint");
}

// union
// enum
// alt

// tuple

// array
// vec

// map
// object
// field

export struct Type {
    enum struct Kind {
        NIL,
        SOME,
        FIELD,

        UNION,
        ENUM,

        ARRAY,
        VEC,

        MAP,
        OBJECT,
    };

    using enum Kind;

    Kind kind;
    Opt<usize> index = NONE;
    Opt<usize> len = NONE;
    Opt<Symbol> tag = NONE;
};

template <typename T>
struct Serde;

export struct Serializer;
export struct Deserializer;

export template <typename T>
Res<> serialize(Serializer& ser, T const& v) {
    if constexpr (requires { v.serialize(ser); }) {
        return v.serialize(ser);
    } else if constexpr (requires { Serde<T>::serialize(ser, v); }) {
        return Serde<T>::serialize(ser, v);
    } else {
        static_assert(false, "No Serde<T> found and T::serialize(Serializer&) is not available");
    }
}

export template <typename T>
Res<T> deserialize(Deserializer& de) {
    if constexpr (requires { T::deserialize(de); }) {
        return T::deserialize(de);
    } else if constexpr (requires { Serde<T>::deserialize(de); }) {
        return Serde<T>::deserialize(de);
    } else {
        static_assert(false, "No Serde<T> found and T::deserialize(Deserialize&) is not available");
    }
}

export struct Serializer {
    virtual ~Serializer() = default;

    virtual Res<> serializeBool(bool v) = 0;
    virtual Res<> serializeUnsigned(u64 v, SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<> serializeInteger(i64 v, SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<> serializeFloat(f64 v, SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<> serializeString(Str v) = 0;
    virtual Res<> serializeBytes(Bytes v) = 0;

    virtual Res<> beginUnit(Type) = 0;
    virtual Res<> endUnit() = 0;

    struct Scope : Meta::NoCopy {
        Serializer* _ser = nullptr;

        Scope(Serializer* ser) : _ser(ser) {}

        Scope(Scope&& other) : _ser(std::exchange(other._ser, nullptr)) {}

        Scope& operator=(Scope&& other) noexcept {
            std::swap(_ser, other._ser);
            return *this;
        }

        ~Scope() {
            (void)end();
        }

        Res<> end() {
            if (_ser) {
                auto res = _ser->endUnit();
                _ser = nullptr;
                return res;
            }
            return Ok();
        }

        Res<> serializeUnit(Type type) {
            return _ser->serializeUnit(type);
        }

        template <typename T>
        Res<> serializeUnit(Type type, T const& v) {
            return _ser->serializeUnit(type, v);
        }

        template <typename T>
        Res<> serialize(T const& v) {
            return _ser->serialize(v);
        }
    };

    Res<Scope> beginScope(Type type) {
        try$(beginUnit(type));
        Scope scope{this};
        return Ok(std::move(scope));
    }

    Res<> serializeUnit(Type type) {
        try$(beginUnit(type));
        return endUnit();
    }

    template <typename T>
    Res<> serializeUnit(Type type, T const& v) {
        auto scope = try$(beginScope(type));
        scope.serialize(v);
        return scope.end();
    }

    template <typename T>
    Res<> serialize(T const& v) {
        return Karm::Serde::serialize<T>(*this, v);
    }
};

export struct Deserializer {
    virtual ~Deserializer() = default;

    virtual Res<bool> deserializeBool() = 0;
    virtual Res<u64> deserializeUnsigned(SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<i64> deserializeInteger(SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<f64> deserializeFloat(SizeHint hint = SizeHint::AUTO) = 0;
    virtual Res<String> deserializeString() = 0;
    virtual Res<Vec<u8>> deserializeBytes() = 0;

    virtual Res<Type> beginUnit(Type) = 0;
    virtual Res<bool> endedUnit() = 0;
    virtual Res<> endUnit() = 0;

    struct Scope {
        Deserializer* _de;
        Type type;

        Scope(Deserializer* de, Type type) : _de(de), type(type) {}

        Scope(Scope&& other) : _de(std::exchange(other._de, nullptr)) {}

        Scope& operator=(Scope&& other) noexcept {
            std::swap(_de, other._de);
            return *this;
        }

        ~Scope() {
            (void)end();
        }

        Res<bool> ended() const {
            return _de->endedUnit();
        }

        Res<> end() {
            if (_de) {
                auto res = _de->endUnit();
                _de = nullptr;
                return res;
            }
            return Ok();
        }

        Res<Type> deserializeUnit(Type type) {
            return _de->deserializeUnit(type);
        }

        template <typename T>
        Res<Tuple<Type, T>> deserializeUnit(Type) {
            return _de->deserializeUnit<Type>(type);
        }

        template <typename T>
        Res<T> deserialize() {
            return _de->deserialize<T>();
        }
    };

    Res<Scope> beginScope(Type type) {
        type = try$(beginUnit(type));
        Scope scope{this, type};
        return Ok(std::move(scope));
    }

    Res<Type> deserializeUnit(Type type) {
        type = try$(beginUnit(type));
        try$(endUnit());
        return Ok(type);
    }

    template <typename T>
    Res<Tuple<Type, T>> deserializeUnit(Type type) {
        auto scope = try$(beginScope(type));
        auto v = try$(scope.deserialize<T>());
        try$(scope.end());
        return Ok<Tuple<Type, T>>();
    }

    template <typename T>
    Res<T> deserialize() {
        return Karm::Serde::deserialize<T>(*this);
    }
};

template <>
struct Serde<bool> {
    static Res<> serialize(Serializer& ser, bool const& v) {
        return ser.serializeBool(v);
    }

    static Res<bool> deserialize(Deserializer& de) {
        return de.deserializeBool();
    }
};

template <>
struct Serde<None> {
    static Res<> serialize(Serializer& ser, None const&) {
        return ser.serializeUnit({Type::NIL});
    }

    static Res<None> deserialize(Deserializer& de) {
        try$(de.deserializeUnit({Type::NIL}));
        return Ok(NONE);
    }
};

template <typename T>
struct Serde<Opt<T>> {
    static Res<> serialize(Serializer& ser, Opt<T> const& v) {
        if (not v)
            return ser.serialize(NONE);
        return ser.serializeUnit({Type::SOME}, v);
    }

    static Res<Opt<T>> deserialize(Deserializer& de) {
        if (auto res = de.deserializeUnit<T>({Type::SOME}))
            return std::move(res);
        return de.deserialize<None>();
    }
};

template <typename V, typename E>
struct Serde<Res<V, E>> {
    static Res<> serialize(Serializer& ser, Res<V, E> const& v) {
        if (not v)
            return ser.serializeUnit<E>({Type::NIL}, v.none());
        return ser.serializeUnit<V>({Type::SOME}, v.unwrap());
    }

    static Res<Res<V, E>> deserialize(Deserializer& de) {
        if (auto res = de.deserializeUnit<V>({Type::SOME}))
            return std::move(res);
        return de.deserializeUnit<E>({Type::NIL});
    }
};

template <typename... Ts>
struct Serde<Union<Ts...>> {
    static Res<> serialize(Serializer& ser, Union<Ts...> const& v) {
        auto scope = try$(ser.beginScope({Type::UNION}));
        try$(v.visit([&](auto const& v) {
            return scope.serializeUnit({.kind = Type::FIELD, .index = v.index()}, v);
        }));
        return scope.end();
    }

    static Res<Union<Ts...>> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({Type::UNION}));
        usize index = 0;
        auto res = try$(Meta::any<Ts...>([&]<typename T> -> Res<Union<Ts...>> {
            return scope.deserializeUnit<T>({.kind = Type::FIELD, .index = index++});
        }));
        try$(scope.end());
        return Ok(std::move(res));
    }
};

template <Meta::Enum T>
struct Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        auto scope = try$(ser.beginScope({Type::ENUM}));
        try$(scope.serializeUnit({.kind = Type::FIELD, .tag = Symbol::from(nameOf(v))}, toUnderlyingType(v)));
        return scope.end();
    }

    static Res<T> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({Type::ENUM}));
        for (auto i : enumItems<T>()) {
            if (auto res = de.deserializeUnit<Meta::UnderlyingType<T>>({.kind = Type::FIELD, .tag = Symbol::from(i.name)})) {
                try$(scope.end());
                return res;
            }
        }
        return Error::invalidData("expected enum value");
    }
};

template <Meta::Unsigned T>
struct Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        return ser.serializeUnsigned(v, sizeHintFor<T>());
    }

    static Res<T> deserialize(Deserializer& de) {
        return de.deserializeUnsigned(sizeHintFor<T>());
    }
};

template <Meta::Signed T>
struct Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        return ser.serializeInteger(v, sizeHintFor<T>());
    }

    static Res<T> deserialize(Deserializer& de) {
        return de.deserializeInteger(sizeHintFor<T>());
    }
};

template <Meta::Float T>
struct Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        return ser.serializeFloat(v, sizeHintFor<T>());
    }

    static Res<T> deserialize(Deserializer& de) {
        return de.deserializeFloat(sizeHintFor<T>());
    }
};

template <>
struct Serde<String> {
    static Res<> serialize(Serializer& ser, String const& v) {
        return ser.serializeString(v);
    }

    static Res<String> deserialize(Deserializer& de) {
        return de.deserializeString();
    }
};

template <>
struct Serde<Vec<u8>> {
    static Res<> serialize(Serializer& ser, Vec<u8> const& v) {
        return ser.serializeBytes(v);
    }

    static Res<Vec<u8>> deserialize(Deserializer& de) {
        return de.deserializeBytes();
    }
};

template <typename T, usize N>
struct Serde<Array<T, N>> {
    static Res<> serialize(Serializer& ser, Array<T, N> const& v) {
        auto scope = try$(ser.beginScope({Type::ARRAY}));
        for (auto& i : v) {
            try$(scope.serialize(i));
        }
        return scope.end();
    }

    static Res<Array<T, N>> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({Type::ARRAY}));
        Array<T, N> res;
        for (auto& i : res) {
            i = try$(scope.deserialize<T>());
        }
        return Ok(std::move(res));
    }
};

template <typename T>
struct Serde<Vec<T>> {
    static Res<> serialize(Serializer& ser, Vec<T> const& v) {
        auto scope = try$(ser.beginScope({.kind = Type::VEC, .len = v.len()}));
        for (auto& i : v) {
            try$(scope->serialize(i));
        }
        return scope.end();
    }

    static Res<Vec<T>> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({Type::VEC}));
        Vec<T> res;
        while (not scope.ended()) {
            res.pushBack(try$(scope.deserialize<T>()));
        }
        try$(scope.end());
        return Ok(std::move(res));
    }
};

template <typename T>
struct Serde<Map<String, T>> {
    static Res<> serialize(Serializer& ser, Map<String, T> const& v) {
        auto scope = try$(ser.beginScope({.kind = Type::MAP, .len = v.len()}));
        for (auto& [k, v] : v.iter())
            try$(scope->serializeUnit({.kind = Type::FIELD, .tag = Symbol::from(k)}, v));
        return scope.end();
    }

    static Res<Map<String, T>> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({.kind = Type::MAP}));
        Map<String, T> res;
        while (not scope.ended()) {
            auto [type, value] = try$(scope.deserializeUnit<T>({.kind = Type::FIELD}));
            res.put(type.tag.unwrap(), value);
        }
        try$(scope.end());
        return Ok(std::move(res));
    }
};

template <typename... Ts>
struct Serde<Tuple<Ts...>> {
    static Res<> serialize(Serializer& ser, Tuple<Ts...> const& v) {
        auto scope = try$(ser.beginScope({.kind = Type::ARRAY}));
        try$(v.visit([&]<typename I>(I const& i) {
            return ser.serialize<I>(i);
        }));
        return scope.end();
    }

    static Res<Tuple<Ts...>> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({.kind = Type::ARRAY}));
        Tuple<Ts...> res;
        try$(res.visit([&]<typename I>(I& i) {
            i = try$(de.deserialize<I>());
            return Ok();
        }));
        try$(scope.end());
        return Ok();
    }
};

template <Meta::Aggregate T>
struct Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        auto scope = try$(ser.beginScope({.kind = Type::ARRAY}));
        try$(visit(v, [&]<typename I>(I const& i) {
            return ser.serialize<I>(i);
        }));
        return scope.end();
    }

    static Res<T> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({.kind = Type::ARRAY}));
        T res;
        try$(visit(res, [&]<typename I>(I& i) {
            i = try$(de.deserialize<I>());
            return Ok();
        }));
        try$(scope.end());
        return Ok();
    }
};

} // namespace Karm::Serde