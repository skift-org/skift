#pragma once

#include <libutils/Scanner.h>
#include <libutils/String.h>

#include <libsystem/json/Array.h>
#include <libsystem/json/Object.h>
#include <libsystem/json/Value.h>
#include <libutils/Prettifier.h>

namespace json
{

Value parse(Scanner &scan);

Value parse(const char *str, size_t size);

Value parse(const String &str);

Value parse_file(const char *path);

void prettify(Prettifier &pretty, const Value &value);

} // namespace json
