#pragma once

#include <libtruetype/Glyph.h>
#include <libutils/Vector.h>

namespace truetype
{

class Font
{
private:
    Vector<Glyph> _glyph;

public:
    Font()
    {
    }

    ~Font()
    {
    }
};

} // namespace truetype
