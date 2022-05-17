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
using Object = Base::Map<Base::String, Value>;
using Array = Base::Vec<Value>;
struct Value : public Base::Var<Object, Array, Base::String, Base::Num, bool, Base::None> {
    using Var::Var;
};

Value parse(Text::Scan &scan);

void emit(Text::Emit &emit, Value const &value);

} // namespace Karm::Json
