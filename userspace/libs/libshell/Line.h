#pragma once

#include <libtext/Rune.h>
#include <libutils/Vec.h>

namespace Shell
{

struct Line : public Vec<Text::Rune>
{
};

} // namespace Shell
