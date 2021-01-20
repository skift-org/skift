#pragma once

#include <libsystem/json/Json.h>

namespace settings
{

struct Bundle
{
    HashMap<String, json::Value> keys;
};

} // namespace settings
