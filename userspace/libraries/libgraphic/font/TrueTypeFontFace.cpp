#include <libgraphic/font/TrueTypeFontFace.h>
#include <libio/FourCC.h>
#include <libio/Skip.h>
#include <libio/Streams.h>

// Font tags
constexpr uint32_t TAG_TrueType_1 = IO::make_tag('1', 0, 0, 0);
constexpr uint32_t TAG_TrueType_Type1 = IO::make_tag('t', 'y', 'p', '1');
constexpr uint32_t TAG_OpenType_1 = IO::make_tag(0, 1, 0, 0);
constexpr uint32_t TAG_OpenType_CFF = IO::make_tag('O', 'T', 'T', 'O');

// File offsets
constexpr size_t TABLE_DIR_OFFSET = 12;

// Table tags
constexpr uint32_t TABLE_TAG_CMAP = IO::make_tag('c', 'm', 'a', 'p');

// TODO: we don't require a memreader here really
ResultOr<RefPtr<Graphic::Font::TrueTypeFontFace>> Graphic::Font::TrueTypeFontFace::load(IO::Reader &reader)
{
    RefPtr<Graphic::Font::TrueTypeFontFace> result;

    auto tag = TRY(IO::read_tag(reader));
    switch (tag)
    {
    case TAG_TrueType_1:
    case TAG_OpenType_1:
        result = make<TrueTypeFontFace>();
        break;

    case TAG_TrueType_Type1:
    case TAG_OpenType_CFF:
        IO::logln("Not supported font formats yet");
        return Result::ERR_NOT_IMPLEMENTED;

    default:
        IO::logln("Unknown FourCC tag for fonts: {}", tag);
        return Result::ERR_INVALID_DATA;
    }

    auto data = TRY(IO::read_all(reader));
    IO::MemoryReader mem_reader(data);
    TRY(result->read_tables(mem_reader));

    return result;
}

Result Graphic::Font::TrueTypeFontFace::read_tables(IO::MemoryReader &reader)
{
    auto num_tables = TRY(IO::read<uint16_t>(reader));
    IO::logln("Font contains {} tables", num_tables);
    TRY(IO::skip(reader, 6));

    for (uint16_t i = 0; i < num_tables; ++i)
    {
        const uint32_t table_offset = TABLE_DIR_OFFSET + 16 * i;
        reader.seek(IO::SeekFrom::start(table_offset));
        auto table_tag = TRY(IO::read_tag(reader));
        switch (table_tag)
        {
        case TABLE_TAG_CMAP:
            IO::logln("CMAP");
            break;
        default:
            IO::logln("Unknown table tag for font: {}", table_tag);
            break;
        }
    }

    return Result::SUCCESS;
}

String Graphic::Font::TrueTypeFontFace::family()
{
    return "";
}

Graphic::Font::FontStyle Graphic::Font::TrueTypeFontFace::style()
{
    return FONT_STYLE_REGULAR;
}

Optional<Graphic::Font::Glyph> Graphic::Font::TrueTypeFontFace::glyph(Codepoint /*c*/)
{
    return {};
}