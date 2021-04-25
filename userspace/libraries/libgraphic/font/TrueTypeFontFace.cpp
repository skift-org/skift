#include <libgraphic/font/TrueTypeFontFace.h>
#include <libio/FourCC.h>
#include <libio/Skip.h>
#include <libio/Streams.h>
#include <libutils/Endian.h>

// Font tags
// TrueType Version 1
constexpr uint32_t TAG_TrueType_V_1 = IO::make_tag('1', 0, 0, 0);
// TrueType with Type 1 font
constexpr uint32_t TAG_TrueType_T_1 = IO::make_tag('t', 'y', 'p', '1');
// OpenType Version 1
constexpr uint32_t TAG_OpenType_V_1 = IO::make_tag(0, 1, 0, 0);
// OpenType with CFF
constexpr uint32_t TAG_OpenType_CFF = IO::make_tag('O', 'T', 'T', 'O');

// Table tags
constexpr uint32_t TABLE_TAG_CMAP = IO::make_tag('c', 'm', 'a', 'p');
constexpr uint32_t TABLE_TAG_LOCA = IO::make_tag('l', 'o', 'c', 'a');
constexpr uint32_t TABLE_TAG_HEAD = IO::make_tag('h', 'e', 'a', 'd');
constexpr uint32_t TABLE_TAG_GLYF = IO::make_tag('g', 'l', 'y', 'f');
constexpr uint32_t TABLE_TAG_HHEA = IO::make_tag('h', 'h', 'e', 'a');
constexpr uint32_t TABLE_TAG_HMTX = IO::make_tag('h', 'm', 't', 'x');
constexpr uint32_t TABLE_TAG_KERN = IO::make_tag('k', 'e', 'r', 'n');
constexpr uint32_t TABLE_TAG_GPOS = IO::make_tag('G', 'P', 'O', 'S');

// Optional table tags
constexpr uint32_t TABLE_TAG_MAXP = IO::make_tag('m', 'a', 'x', 'p');

// TODO: we don't require a memreader here really
ResultOr<RefPtr<Graphic::Font::TrueTypeFontFace>> Graphic::Font::TrueTypeFontFace::load(IO::Reader &reader)
{
    RefPtr<Graphic::Font::TrueTypeFontFace> result;

    auto tag = TRY(IO::read_tag(reader));
    switch (tag())
    {
    case TAG_TrueType_V_1:
    case TAG_OpenType_V_1:
        result = make<TrueTypeFontFace>();
        break;

    case TAG_TrueType_T_1:
    case TAG_OpenType_CFF:
        IO::logln("Not supported font formats yet");
        return Result::ERR_NOT_IMPLEMENTED;

    default:
        IO::logln("Unknown FourCC tag for fonts: {}", tag());
        return Result::ERR_INVALID_DATA;
    }

    auto data = TRY(IO::read_all(reader));
    IO::MemoryReader mem_reader(data);
    TRY(result->read_tables(mem_reader));

    if (!result->_has_cmap || !result->_has_head || !result->_has_hhea || !result->_has_hmtx)
    {
        IO::errln("Font is missing a required table!");
        return Result::ERR_INVALID_DATA;
    }

    if (result->_has_glyf)
    {
        // required for truetype
        if (!result->_has_loca)
        {
            return Result::ERR_INVALID_DATA;
        }
    }
    else
    {
    }

    IO::logln("Font has {} glyphs", result->_num_glyphs);

    return result;
}

Result Graphic::Font::TrueTypeFontFace::read_tables(IO::MemoryReader &reader)
{
    Assert::equal(TRY(reader.tell()), 0);
    auto num_tables = TRY(IO::read<le_uint16_t>(reader));
    IO::logln("Font contains {} tables", num_tables());
    TRY(IO::skip(reader, 6));

    for (uint16_t i = 0; i < num_tables(); i++)
    {
        // Read the table entry
        auto table_tag = TRY(IO::read_tag(reader));
        auto table_checksum = TRY(IO::read<le_uint32_t>(reader));
        auto table_offset = TRY(IO::read<le_uint32_t>(reader));
        auto table_size = TRY(IO::read<le_uint32_t>(reader));

        // Jump to the table position and create our readers
        auto old_pos = TRY(reader.tell());
        reader.seek(IO::SeekFrom::start(table_offset()));
        IO::ScopedReader table_reader(reader, table_size());

        // TODO: validate the checksum
        UNUSED(table_checksum);

        switch (table_tag())
        {
        case TABLE_TAG_CMAP:
            IO::logln("CMAP: {}", table_offset());
            _has_cmap = true;
            break;
        case TABLE_TAG_LOCA:
            IO::logln("LOCA: {}", table_offset());
            _has_loca = true;
            break;
        case TABLE_TAG_HEAD:
            IO::logln("HEAD: {}", table_offset());
            _has_head = true;
            break;
        case TABLE_TAG_GLYF:
            IO::logln("GLYF: {}", table_offset());
            _has_glyf = true;
            break;
        case TABLE_TAG_HHEA:
            IO::logln("HHEA: {}", table_offset());
            _has_hhea = true;
            break;
        case TABLE_TAG_HMTX:
            IO::logln("HMTX: {}", table_offset());
            _has_hmtx = true;
            break;
        case TABLE_TAG_MAXP:
            read_table_maxp(table_reader);
            break;
        default:
            // IO::logln("Unknown table tag for font: {}", table_tag);
            break;
        }

        reader.seek(IO::SeekFrom::start(old_pos));
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

Result Graphic::Font::TrueTypeFontFace::read_table_cmap(IO::Reader & /*reader*/)
{
    return Result::SUCCESS;
}

Result Graphic::Font::TrueTypeFontFace::read_table_maxp(IO::Reader &reader)
{
    auto major = TRY(IO::read<le_uint16_t>(reader));
    auto minor = TRY(IO::read<le_uint16_t>(reader));

    _num_glyphs = TRY(IO::read<le_uint16_t>(reader))();

    // Version 1.0 has extra data
    if (major() == 1 && minor() == 0)
    {
        // TODO: read extra data for version 1.0
        // https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
    }

    return Result::SUCCESS;
}