#pragma once

#include <karm-base/map.h>
#include <karm-base/string.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-fmt/fmt.h>
#include <karm-text/emit.h>
#include <karm-text/expr.h>
#include <karm-text/scan.h>

namespace Json {

struct Value;

using Array = Vec<Value>;

using Object = Map<String, Value>;

using Store = Var<None, Array, Object, String, double, bool>;

struct Value {
    Store _store;

    Value() : _store(NONE) {}
    Value(None) : _store(NONE) {}
    Value(Array v) : _store(v) {}
    Value(Object m) : _store(m) {}
    Value(String s) : _store(s) {}
    Value(double d) : _store(d) {}
    Value(bool b) : _store(b) {}

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

    Value &operator=(double d) {
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
        return _store.is<double>();
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
                    return "<null>";
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
                [](int i) {
                    return Fmt::format("{}", i);
                },
                [](double d) {
                    return Fmt::format("{}", d);
                },
                [](bool b) {
                    return b ? "true" : "false";
                },
            });
    }

    int asInt() const {
        return _store.visit(
            Visitor{
                [](int i) {
                    return i;
                },
                [](double d) {
                    return (int)d;
                },
                [](bool b) {
                    return b ? 1 : 0;
                },
                [](auto) {
                    return 0.0;
                },
            });
    }

    double asFloat() const {
        return _store.visit(
            Visitor{
                [](int i) {
                    return (double)i;
                },
                [](double d) {
                    return d;
                },
                [](bool b) {
                    return b ? 1.0 : 0.0;
                },
                [](auto) {
                    return 0.0;
                },
            });
    }

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
                [](int i) {
                    return i != 0;
                },
                [](double d) {
                    return d != 0.0;
                },
                [](bool b) {
                    return b;
                },
            });
    }

    Opt<Value> get(Str key) const {
        if (not isObject()) {
            return NONE;
        }
        return asObject().get(key);
    }

    Opt<Value> get(size_t index) const {
        if (not isArray() or asArray().len() <= index) {
            return NONE;
        }
        return asArray()[index];
    }

    size_t len() const {
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
                [](int) {
                    return 0;
                },
                [](double) {
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
};

Result<Value> parse(Text::Scan &s);

Result<Value> parse(Str s);

Error stringify(Text::Emit &emit, Value const &v);

Result<String> stringify(Value const &v);

} // namespace Json

template <>
struct Karm::Fmt::Formatter<Json::Value> {
    Result<size_t> format(Io::_TextWriter &writer, Json::Value value) {
        Text::Emit emit{writer};
        try$(Json::stringify(emit, value));
        return emit.total();
    }
};
