#pragma once

#include <libutils/Vector.h>
#include <libutils/unicode/Codepoint.h>

namespace Shell
{

struct Line : public Vector<Codepoint>
{
};

} // namespace Shell
