#pragma once

#include <karm-base/map.h>
#include <karm-io/emit.h>

namespace Karm::Pdf {

struct Value;

struct Ref {
    usize num{};
    usize gen{};

    Ref alloc() {
        return {++num, gen};
    }

    bool operator==(Ref const& other) const = default;
    auto operator<=>(Ref const& other) const = default;
};

struct Name : String {
    using String::String;

    void write(Io::Emit& e) const;
};

struct Array : Vec<Value> {
    using Vec<Value>::Vec;

    void write(Io::Emit& e) const;
};

struct Dict : Map<Name, Value> {
    using Map<Name, Value>::Map;

    void write(Io::Emit& e) const;
};

struct Stream {
    Dict dict;
    Buf<Byte> data;

    void write(Io::Emit& e) const;
};

using _Value = Union<
    None,
    Ref,
    bool,
    isize,
    usize,
    f64,
    String,
    Name,
    Array,
    Dict,
    Stream>;

struct Value : _Value {
    using _Value::_Value;

    void write(Io::Emit& e) const;
};

struct File {
    String header;
    Map<Ref, Value> body;
    Dict trailer;

    Ref add(Ref ref, Value Value) {
        body.put(ref, Value);
        return ref;
    }

    Res<> write(Io::Writer& e) const;
};

struct XRef {
    struct Entry {
        usize offset;
        usize gen;
        bool used;
    };

    Vec<Entry> entries;

    void add(usize offset, usize gen) {
        entries.pushBack({offset, gen, true});
    }

    void write(Io::Emit& e) const;
};

} // namespace Karm::Pdf
