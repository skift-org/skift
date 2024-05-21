#pragma once

#include <karm-base/map.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-io/bscan.h>
#include <karm-math/rect.h>

namespace Pdf {

struct Object;

// MARK: Values ----------------------------------------------------------------

struct Ref {
    usize num;
    usize gen;
};

using _Value = Union<
    None,
    bool,
    i64,
    f64,
    Ref,
    Strong<Object>>;

struct Value : public _Value {
    using _Value::_Value;
};

// MARK: Objects ---------------------------------------------------------------

struct String : public Karm::String {
    usize gen = 0;

    using Karm::String::String;
};

struct Name : public Karm::String {
    usize gen = 0;

    using Karm::String::String;
};

struct Array : public Vec<Value> {
    usize gen = 0;

    using Vec<Value>::Vec;
};

struct Dict : public Map<String, Value> {
    usize gen = 0;

    using Map<String, Value>::Map;
};

struct Stream : public Buf<Byte> {
    usize gen = 0;

    using Buf<Byte>::Buf;
};

struct Indirect : public Value {
    usize gen = 0;

    using Value::Value;
};

using _Object = Union<
    String,
    Name,
    Array,
    Dict,
    Stream,
    Indirect>;

struct Object : public _Object {
    using _Object::_Object;
};

// MARK: Pages  & Document -----------------------------------------------------

struct Page {
    Dict resources;
    Strong<Object> contents;
    Math::Rectf mediaBox;
    Math::Rectf cropBox;
};

struct Document {
    Map<usize, Page> pages;
};

// MARK: Parsing & Emitting ----------------------------------------------------

Res<> parse(Io::BScan &s, Document &doc);

Res<> emit(Io::BEmit &e, Document const &doc);

} // namespace Pdf
