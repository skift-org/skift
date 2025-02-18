#pragma once

#include <karm-base/set.h>
#include <karm-base/string.h>
#include <karm-base/union.h>
#include <karm-gc/ptr.h>
#include <karm-io/emit.h>

struct Agent;

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-ecmascript-language-types-undefined-type
struct Undefined {
    bool operator==(Undefined const&) const = default;
    auto operator<=>(Undefined const&) const = default;

    void repr(Io::Emit& e) const {
        e("undefined");
    }
};

static constexpr Undefined undefined{};

// https://tc39.es/ecma262/#sec-ecmascript-language-types-null-type
struct Null {
    bool operator==(Null const&) const = default;
    auto operator<=>(Null const&) const = default;

    void repr(Io::Emit& e) const {
        e("null");
    }
};

static constexpr Null null{};

// https://tc39.es/ecma262/#sec-ecmascript-language-types-boolean-type
using Boolean = bool;

// https://tc39.es/ecma262/#sec-ecmascript-language-types-string-type
using String = _String<Utf16>;

// https://tc39.es/ecma262/#sec-ecmascript-language-types-symbol-type
struct Symbol {
    // FIXME: That's not how symbols work. We need to implement a symbol table.
    String _desc;

    static Symbol make(String desc) {
        return {std::move(desc)};
    }

    bool operator==(Symbol const& other) const {
        return _desc == other._desc;
    }

    void repr(Io::Emit& e) const {
        e("(Symbol {#})", _desc);
    }
};

// MARK: Numeric Types ---------------------------------------------------------
// https://tc39.es/ecma262/#sec-ecmascript-language-types-number-type

struct Number {
    f64 _val;

    Number(f64 val) : _val{val} {}

    // https://tc39.es/ecma262/#sec-numeric-types-number-unaryMinus
    Number unaryMinus();

    // https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseNOT
    Number bitwiseNot();

    // https://tc39.es/ecma262/#sec-numeric-types-number-exponentiate
    Number exponentiate(Number exp);

    // https://tc39.es/ecma262/#sec-numeric-types-number-multiply
    Number multiply(Number multiplicand);

    // https://tc39.es/ecma262/#sec-numeric-types-number-divide
    Number divide(Number divisor);

    // https://tc39.es/ecma262/#sec-numeric-types-number-remainder
    Number remainder(Number divisor);

    // https://tc39.es/ecma262/#sec-numeric-types-number-add
    Number add(Number augend);

    // https://tc39.es/ecma262/#sec-numeric-types-number-subtract
    Number subtract(Number subtrahend);

    // https://tc39.es/ecma262/#sec-numeric-types-number-leftShift
    Number leftShift(Number shiftCount);

    // https://tc39.es/ecma262/#sec-numeric-types-number-signedRightShift
    Number signedRightShift(Number shiftCount);

    // https://tc39.es/ecma262/#sec-numeric-types-number-unsignedRightShift
    Number unsignedRightShift(Number shiftCount);

    // https://tc39.es/ecma262/#sec-numeric-types-number-lessThan
    Boolean lessThan(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-equal
    Boolean equal(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-sameValue
    Boolean sameValue(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-sameValueZero
    Boolean sameValueZero(Number y);

    // https://tc39.es/ecma262/#sec-numberbitwiseop
    Number bitwiseOp(Number y, f64 op);

    // https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseAND
    Number bitwiseAnd(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseXOR
    Number bitwiseXor(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-bitwiseOR
    Number bitwiseOr(Number y);

    // https://tc39.es/ecma262/#sec-numeric-types-number-tostring
    String toString();

    void repr(Io::Emit& e) const;
};

struct Object;

// MARK: Value Type ------------------------------------------------------------
// https://tc39.es/ecma262/#sec-ecmascript-language-types

struct Value {
    using _Store = Union<
        Undefined,
        Null,
        Boolean,
        String,
        Symbol,
        Number,
        Gc::Ref<Object>>;

    _Store store = undefined;

    Value() = default;

    template <Meta::Convertible<_Store> T>
    Value(T&& t) : store{std::forward<T>(t)} {}

    Value(Gc::Ptr<Object> ptr)
        : store{
              ptr
                  ? _Store{ptr.upgrade()}
                  : null
          } {}

    template <Meta::Equatable<_Store> T>
    bool operator==(T const& t) const {
        return store == t;
    }

    bool isBoolean() const {
        return store.is<Boolean>();
    }

    Boolean asBoolean() const {
        return store.unwrap<Boolean>();
    }

    bool isString() const {
        return store.is<String>();
    }

    String asString() const {
        return store.unwrap<String>();
    }

    bool isSymbol() const {
        return store.is<Symbol>();
    }

    Symbol asSymbol() const {
        return store.unwrap<Symbol>();
    }

    bool isNumber() const {
        return store.is<Number>();
    }

    Number asNumber() const {
        return store.unwrap<Number>();
    }

    Boolean isObject() const {
        return store.is<Gc::Ref<Object>>();
    }

    Gc::Ref<Object> asObject() {
        return store.unwrap<Gc::Ref<Object>>();
    }

    void repr(Io::Emit& e) const;
};

} // namespace Vaev::Script
