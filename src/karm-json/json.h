#pragma once

#include <karm-base/map.h>
#include <karm-base/num.h>
#include <karm-base/string.h>
#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-text/emit.h>
#include <karm-text/scan.h>

namespace Karm::Json {

struct Value;
using Object = Map<String, Value>;
using Array = Vec<Value>;
struct Value : public Var<Object, Array, String, Num, bool, None> {
    using Var::Var;
};

Value parse(Text::Scan &scan);

void emit(Text::Emit &emit, Value const &value);

} // namespace Karm::Json
