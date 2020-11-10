#pragma once

#include <libsystem/json/Value.h>
#include <libutils/Vector.h>

namespace json
{

struct Array : public Vector<Value>
{
};

} // namespace json
