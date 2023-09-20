#pragma once

#include <karm-base/map.h>
#include <karm-base/string.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-fmt/fmt.h>
#include <karm-io/emit.h>
#include <karm-io/expr.h>
#include <karm-io/sscan.h>

namespace Json {

struct Value;

using Array = Vec<Value>;

using Object = Map<String, Value>;

#ifdef __ck_freestanding__
using Number = isize;
#else
using Number = f64;
#endif

using Store = Var<None, Array, Object, String, Number, bool>;

struct Value {
    Store _store;

    Value()
        : _store(NONE) {}

    Value(None)
        : _store(NONE) {}

    Value(Array v)
        : _store(v) {}

    Value(Object m)
        : _store(m) {}

    Value(String s)
        : _store(s) {}

    Value(Number d)
        : _store(d) {}

    Value(bool b)
        : _store(b) {}

    Value &operator=(None) {
        _store = NONE;
        return *this;
    }

    Value &operator=(Array v) {
        _store = v;
        return *this;
    }

    Value &operator=(Object m) {
        _store = m;
        return *this;
    }

    Value &operator=(String s) {
        _store = s;
        return *this;
    }

    Value &operator=(Number d) {
        _store = d;
        return *this;
    }

    Value &operator=(bool b) {
        _store = b;
        return *this;
    }

    bool isNull() const {
        return _store.is<None>();
    }

    bool isArray() const {
        return _store.is<Vec<Value>>();
    }

    bool isObject() const {
        return _store.is<Object>();
    }

    bool isStr() const {
        return _store.is<String>();
    }

    bool isFloat() const {
        return _store.is<Number>();
    }

    bool isBool() const {
        return _store.is<bool>();
    }

    Array &asArray() {
        return _store.unwrap<Array>();
    }

    Array const &asArray() const {
        return _store.unwrap<Array>();
    }

    Object &asObject() {
        return _store.unwrap<Object>();
    }

    Object const &asObject() const {
        return _store.unwrap<Object>();
    }

    String asStr() const {
        return _store.visit(
            Visitor{
                [](None) {
                    return "null";
                },
                [](Vec<Value>) {
                    return "<array>";
                },
                [](Map<String, Value>) {
                    return "<object>";
                },
                [](String s) {
                    return s;
                },
                [](Number d) {
                    return Fmt::format("{}", d).unwrap();
                },
                [](bool b) {
                    return b ? "true" : "false";
                },
            });
    }

    isize asInt() const {
        return _store.visit(
            Visitor{
                [](Number d) {
                    return (isize)d;
                },
                [](bool b) {
                    return b ? 1 : 0;
                },
                [](auto) {
                    return 0;
                },
            });
    }

#ifdef __ck_freestanding__
    f64 asFloat() const {
        return _store.visit(
            Visitor{
                [](Number d) {
                    return d;
                },
                [](bool b) {
                    return b ? (Number)1.0 : (Number)0.0;
                },
                [](auto) {
                    return (Number)0;
                },
            });
    }
#endif

    bool asBool() const {
        return _store.visit(
            Visitor{
                [](None) {
                    return false;
                },
                [](Vec<Value> v) {
                    return v.len() > 0;
                },
                [](Map<String, Value> m) {
                    return m.len() > 0;
                },
                [](String s) {
                    return s.len() > 0;
                },
                [](Number d) {
                    return d != (Number)0;
                },
                [](bool b) {
                    return b;
                },
            });
    }

    Value get(Str key) const {
        if (not isObject()) {
            return NONE;
        }
        return try$(asObject().get(key));
    }

    Value get(usize index) const {
        if (not isArray() or asArray().len() <= index) {
            return NONE;
        }
        return asArray()[index];
    }

    usize len() const {
        return _store.visit(
            Visitor{
                [](None) {
                    return 0;
                },
                [](Vec<Value> v) {
                    return v.len();
                },
                [](Map<String, Value> m) {
                    return m.len();
                },
                [](String s) {
                    return s.len();
                },
                [](Number) {
                    return 0;
                },
                [](bool) {
                    return 0;
                },
            });

        return 0;
    }

    void clear() {
        _store.visit(
            Visitor{
                [](auto &v) {
                    v = {};
                }});
    }

    auto visit(auto visitor) const {
        return _store.visit(visitor);
    }

    template <typename T>
    Opt<T> take() {
        return _store.take<T>();
    }

    template <typename T>
    Opt<T> take() const {
        return _store.take<T>();
    }
};

Res<Value> parse(Io::SScan &s);

Res<Value> parse(Str s);

Res<> stringify(Io::Emit &emit, Value const &v);

Res<String> stringify(Value const &v);

} // namespace Json

inline auto operator""_json(char const *str, usize len) {
    return Json::parse({str, len}).unwrap();
}

template <>
struct Karm::Fmt::Formatter<Json::Value> {
    Res<usize> format(Io::TextWriter &writer, Json::Value value) {
        Io::Emit emit{writer};
        try$(Json::stringify(emit, value));
        return Ok(emit.total());
    }
};
