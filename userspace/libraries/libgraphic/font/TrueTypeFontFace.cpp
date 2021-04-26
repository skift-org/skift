#include <libgraphic/font/TrueTypeFontFace.h>
#include <libio/FourCC.h>
#include <libio/ReadCounter.h>
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
    auto data = TRY(IO::read_all(reader));
    IO::MemoryReader mem_reader(data);

    auto tag = TRY(IO::read_tag(mem_reader));
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
    auto num_tables = TRY(IO::read<be_uint16_t>(reader));
    IO::logln("Font contains {} tables", num_tables());

    // We don't really need any of these
    auto search_range = TRY(IO::read<be_uint16_t>(reader));
    auto entry_selector = TRY(IO::read<be_uint16_t>(reader));
    auto range_shift = TRY(IO::read<be_uint16_t>(reader));
    UNUSED(search_range);
    UNUSED(entry_selector);
    UNUSED(range_shift);

    for (uint16_t i = 0; i < num_tables(); i++)
    {
        // Read the table entry
        auto table_tag = TRY(IO::read_tag(reader));
        auto table_checksum = TRY(IO::read<be_uint32_t>(reader));
        auto table_offset = TRY(IO::read<be_uint32_t>(reader));
        auto table_size = TRY(IO::read<be_uint32_t>(reader));

        // Jump to the table position and create our readers
        auto old_pos = TRY(reader.tell());
        Assert::lower_equal((size_t)table_offset(), TRY(reader.length()));

        reader.seek(IO::SeekFrom::start(table_offset()));
        IO::ScopedReader table_reader(reader, table_size());

        // TODO: validate the checksum
        UNUSED(table_checksum);

        switch (table_tag())
        {
        case TABLE_TAG_CMAP:
            read_table_cmap(table_reader);
            break;
        case TABLE_TAG_LOCA:
            IO::logln("LOCA: {}", table_offset());
            _has_loca = true;
            break;
        case TABLE_TAG_HEAD:
            read_table_head(table_reader);
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

ResultOr<Graphic::Font::TrueTypeVersion> Graphic::Font::TrueTypeFontFace::read_version(IO::Reader &reader)
{
    auto major = TRY(IO::read<be_uint16_t>(reader));
    auto minor = TRY(IO::read<be_uint16_t>(reader));

    return TrueTypeVersion{major, minor};
}

Result Graphic::Font::TrueTypeFontFace::read_table_cmap(IO::Reader &_reader)
{
    auto reader = IO::ReadCounter(_reader);
    _has_cmap = true;
    auto version = TRY(IO::read<uint16_t>(reader));
    UNUSED(version);
    auto num_encodings = TRY(IO::read<be_uint16_t>(reader));
    IO::logln("CMAP num_encodings: {}", num_encodings());

    uint32_t encoding_table_offset = 0;

    // Find an encoding we can work with (Unicode)
    for (uint16_t i = 0; i < num_encodings() && encoding_table_offset == 0; i++)
    {
        auto platform_id = TRY(IO::read<be_uint16_t>(reader));
        auto encoding_id = TRY(IO::read<be_uint16_t>(reader));
        UNUSED(encoding_id);
        auto subtable_offset = TRY(IO::read<be_uint32_t>(reader));

        switch (platform_id())
        {
        case TT_PLATFORM_WINDOWS:
            if (encoding_id() == TT_ENCODING_WIN_UNICODE_BMP ||
                encoding_id() == TT_ENCODING_WIN_UNICODE_FULL)
            {
                encoding_table_offset = subtable_offset();
            }
            break;
        case TT_PLATFORM_UNICODE:
        {
            encoding_table_offset = subtable_offset();
        }
        default:
            break;
        }

        if (encoding_table_offset == 0)
        {
            IO::logln("Skipped unsupported encoding table (Platform: {} Encoding: {})", platform_id(), encoding_id());
        }
        else
        {
            IO::logln("Using encoding table (Platform: {} Encoding: {})", platform_id(), encoding_id());
        }
    }

    if (encoding_table_offset == 0)
    {
        IO::errln("Found no supported encoding table");
        return Result::ERR_INVALID_DATA;
    }

    // Skip to our position
    auto to_skip = encoding_table_offset - reader.count();
    IO::logln("Skipping {} bytes to subtable", to_skip);
    TRY(IO::skip(reader, to_skip));

    // Read the character mapping
    reader.reset();
    auto mapping_format = TRY(IO::read<be_uint16_t>(reader));
    IO::logln("Character mapping format: {}", mapping_format());
    auto length = TRY(IO::read<be_uint16_t>(reader));   // length of table in bytes
    auto language = TRY(IO::read<be_uint16_t>(reader)); // language
    UNUSED(language);

    switch (mapping_format())
    {
    case TT_MAPPING_FMT_APPLE_BYTE_ENCODING:
    {
        // We already the first 6 bytes of this table
        for (uint16_t i = 0; i < (length() - reader.count()) && i < 256; i++)
        {
            _codepoint_glyph_mapping[i] = TRY(IO::read<be_uint8_t>(reader))();
        }
    }
    break;
    case TT_MAPPING_FMT_SEGMENT:
    {
        auto seg_count = TRY(IO::read<be_uint16_t>(reader))() / 2;
        auto search_range = TRY(IO::read<be_uint16_t>(reader));
        UNUSED(search_range);
        auto entry_selector = TRY(IO::read<be_uint16_t>(reader));
        UNUSED(entry_selector);
        auto range_shift = TRY(IO::read<be_uint16_t>(reader));
        UNUSED(range_shift);

        auto end_codes = TRY(IO::read_vector<be_uint16_t>(reader, seg_count));
        TRY(IO::skip(reader, sizeof(uint16_t)));
        auto start_codes = TRY(IO::read_vector<be_uint16_t>(reader, seg_count));
        auto deltas = TRY(IO::read_vector<be_uint16_t>(reader, seg_count));
        auto range_offsets = TRY(IO::read_vector<be_uint16_t>(reader, seg_count));

        auto header_length = 16 + (seg_count * 8);
        auto num_glyphs = (length() - header_length) / sizeof(uint16_t);
        auto glyph_ids = TRY(IO::read_vector<be_uint16_t>(reader, num_glyphs));

        IO::logln("SegCount: {}", seg_count);

        // Read all segments, last segment is a pseudo-segment (start & end both set to 0xFFFF)
        for (uint16_t current_seg = 0; current_seg < (seg_count - 1); current_seg++)
        {
            const auto &start_code = start_codes[current_seg]();
            const auto &end_code = end_codes[current_seg]();
            const auto &delta = deltas[current_seg]();
            const auto &range_offset = range_offsets[current_seg]();
            IO::logln("StartCode: {}, EndCode: {}, RangeOffset: {}", start_code, end_code, range_offset);

            auto range = end_code - start_code;

            for (uint16_t i = 0; i <= range; i++)
            {
                uint16_t glyph_index = 0;
                if (range_offset == 0)
                {
                    glyph_index = start_code + i + delta;
                }
                else
                {
                    glyph_index = glyph_ids[(range_offset / 2) + i - seg_count + current_seg]();
                }

                _codepoint_glyph_mapping[start_code + i] = glyph_index;
                IO::logln("Mapped {} to {}", start_code + i, glyph_index);
            }
        }
    }
    break;
    default:
        IO::errln("Unsupported mapping format: {}", mapping_format());
        return Result::ERR_NOT_IMPLEMENTED;
    }

    return Result::SUCCESS;
}

Result Graphic::Font::TrueTypeFontFace::read_table_head(IO::Reader &reader)
{
    auto header = TRY(IO::read<TrueTypeHeader>(reader));
    UNUSED(header);

    return Result::SUCCESS;
}

Result Graphic::Font::TrueTypeFontFace::read_table_maxp(IO::Reader &reader)
{
    auto version = TRY(read_version(reader));

    _num_glyphs = TRY(IO::read<be_uint16_t>(reader))();

    // Version 1.0 has extra data
    if (version.x()() == 1 && version.y()() == 0)
    {
        // TODO: read extra data for version 1.0
        // https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
    }

    return Result::SUCCESS;
}