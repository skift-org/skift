#pragma once

#include <karm-base/map.h>
#include <karm-io/emit.h>

namespace Pdf {

struct Object;

struct Ref {
    usize num;
    usize gen;
};

struct Name {
    String value;
};

using Array = Vec<Object>;

using Dict = Map<Name, Object>;

struct Stream {
    Dict dict;
    Bytes data;
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

} // namespace Pdf
