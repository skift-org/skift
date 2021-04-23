#pragma once

#include <libgraphic/font/FontFace.h>
#include <libio/Reader.h>
#include <libutils/unicode/Codepoint.h>

namespace Graphic::Font
{

class TrueTypeFontFace : public FontFace
{
private:
    Result read_tables(IO::MemoryReader& reader); 
public:
    static ResultOr<RefPtr<TrueTypeFontFace>> load(IO::Reader& reader);

    virtual String family() override;
    virtual FontStyle style() override;
    virtual Optional<Glyph> glyph(Codepoint c) override;
};
} // namespace Graphic::Font