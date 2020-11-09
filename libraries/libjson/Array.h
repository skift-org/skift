#pragma once

#include <libjson/Value.h>
#include <libutils/Vector.h>

namespace json
{

struct Array : public Vector<Value>
{
};

} // namespace json
