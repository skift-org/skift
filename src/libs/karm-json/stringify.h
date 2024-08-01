#pragma once

#include "values.h"

namespace Karm::Json {

Res<> stringify(Io::Emit &emit, Value const &v);

Res<String> stringify(Value const &v);

} // namespace Karm::Json

template <>
struct Karm::Io::Formatter<Karm::Json::Value> {
    Res<usize> format(Io::TextWriter &writer, Karm::Json::Value value) {
        Io::Emit emit{writer};
        try$(Karm::Json::stringify(emit, value));
        return Ok(emit.total());
    }
};
