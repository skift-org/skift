#pragma once

#include <libsystem/json/Value.h>
#include <libutils/HashMap.h>
#include <libutils/String.h>

namespace json
{

struct Object : public HashMap<String, Value>
{
};

} // namespace json
