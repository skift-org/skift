#pragma once

#include <libutils/Scanner.h>
#include <libutils/String.h>

#include <libsystem/json/Array.h>
#include <libsystem/json/Object.h>
#include <libsystem/json/Value.h>

namespace json
{

String stringify(const Value &value);

String prettify(const Value &value); // *WITH* beautiful vt100 colors.

Value parse(Scanner &scan);

Value parse(const char *str, size_t size);

Value parse_file(const char *path);

} // namespace json
