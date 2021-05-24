#pragma once

#include <libtext/Rune.h>
#include <libutils/Vector.h>

namespace Shell
{

struct Line : public Vector<Text::Rune>
{
};

} // namespace Shell
