#pragma once

#include <libutils/Scanner.h>
#include <libutils/String.h>

#include <libutils/json/Parser.h>
#include <libutils/json/Prettifier.h>
#include <libutils/json/Value.h>

namespace json
{

Value parse_file(String path);

} // namespace json
