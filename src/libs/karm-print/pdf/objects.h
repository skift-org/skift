#pragma once

#include <karm-base/map.h>
#include <karm-io/emit.h>

namespace Pdf {

struct Object;

struct Ref {
    usize num{};
    usize gen{};

    Ref alloc() {
        return {++num, gen};
    }

    bool operator==(Ref const &other) const = default;
    auto operator<=>(Ref const &other) const = default;
};

struct Name : public String {
    using String::String;
};

using Array = Vec<Object>;

using Dict = Map<Name, Object>;

struct Stream {
    Dict dict;
    Buf<Byte> data;
};

using _Object = Union<
    None,
    Ref,
    bool,
    isize,
    f64,
    String,
    Name,
    Array,
    Dict,
    Stream>;

struct Object : public _Object {
    using _Object::_Object;
};

void write(Io::Emit &e, Object const &o);

struct File {
    String header;
    Map<Ref, Object> body;
    Dict trailer;

    Ref add(Ref ref, Object object) {
        body.put(ref, object);
        return ref;
    }
};

void write(Io::Emit &e, File const &o);

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
};

void write(Io::Emit &e, XRef const &xref);

} // namespace Pdf
