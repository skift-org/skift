#pragma once

#include <karm-io/emit.h>

#include "length.h"
#include "numbers.h"

namespace Vaev {

enum struct CalcOp {
    NOP,

    ADD,
    SUBSTRACT,
    MULTIPLY,
    DIVIDE,
    SIN,
    TAN,
    COS,

    _LEN
};

// 10. Mathematical Expressions
// https://drafts.csswg.org/css-values/#math
template <typename T>
struct CalcValue {
    using Leaf = Box<CalcValue<T>>;

    using Value = Union<
        T,
        Leaf,
        Number>;

    struct Unary {
        CalcOp op = CalcOp::NOP;
        Value val;
    };

    struct Binary {
        CalcOp op = CalcOp::NOP;
        Value lhs;
        Value rhs;
    };

    using Inner = Union<
        Value,
        Box<Unary>,
        Box<Binary>>;

    Inner _inner;

    constexpr CalcValue()
        : CalcValue(T{}) {
    }

    constexpr CalcValue(Meta::Convertible<T> auto val)
        : _inner(Value{T{val}}) {
    }

    constexpr CalcValue(Value val)
        : _inner(val) {
    }

    constexpr CalcValue(CalcOp op, Value val)
        : _inner(makeBox<Unary>(op, val)) {
    }

    constexpr CalcValue(CalcOp op, Value lhs, Value rhs)
        : _inner(makeBox<Binary>(op, lhs, rhs)) {
    }

    auto visit(this auto& self, auto visitor) {
        return self._inner.visit(Visitor{
            [&](Value const& v) {
                return visitor(v);
            },
            [&](Box<Unary> const& u) {
                return visitor(*u);
            },
            [&](Box<Binary> const& b) {
                return visitor(*b);
            },
        });
    }

    void repr(Io::Emit& e) const {
        visit(Visitor{
            [&](Value const& v) {
                e("{}", v);
            },
            [&](Unary const& u) {
                e("(calc {} {})", u.op, u.val);
            },
            [&](Binary const& b) {
                e("(calc {} {} {})", b.op, b.lhs, b.rhs);
            },
        });
    }
};

} // namespace Vaev
