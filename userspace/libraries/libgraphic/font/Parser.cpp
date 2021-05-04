#include <libgraphic/font/Parser.h>
#include <libgraphic/font/TrueTypeFontFace.h>

ResultOr<RefPtr<Graphic::SuperCoolFont::FontFace>> Graphic::SuperCoolFont::load(IO::Reader &reader)
{
    RefPtr<FontFace> result = TrueTypeFontFace::load(reader).unwrap();
    return result;
}
