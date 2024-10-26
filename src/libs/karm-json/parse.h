#pragma once

#include <karm-io/sscan.h>

#include "values.h"

namespace Karm::Json {

Res<Value> parse(Io::SScan &s);

Res<Value> parse(Str s);

} // namespace Karm::Json

inline auto operator""_json(char const *str, usize len) {
    return Karm::Json::parse({str, len}).unwrap();
}
