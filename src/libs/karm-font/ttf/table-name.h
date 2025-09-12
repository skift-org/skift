#pragma once

import Karm.Core;

namespace Karm::Font::Ttf {

struct Name : Io::BChunk {
    static constexpr Str SIG = "name";

    enum NameId : u16 {
        COPYRIGHT = 0,
        FAMILY = 1,
        SUBFAMILY = 2,
        UNIQUE = 3,
        FULL_NAME = 4,
        VERSION = 5,
        POSTSCRIPT = 6,
        TRADEMARK = 7,
        MANUFACTURER = 8,
        DESIGNER = 9,
        DESCRIPTION = 10,
        VENDOR_URL = 11,
        DESIGNER_URL = 12,
        LICENSE = 13,
        LICENSE_URL = 14,
        RESERVED = 15,
        TYPOGRAPHIC_FAMILY = 16,
        TYPOGRAPHIC_SUBFAMILY = 17,
        COMPATIBLE_FULL_NAME = 18,
        SAMPLE_TEXT = 19,
        POSTSCRIPT_CID = 20,
        WWS_FAMILY = 21,
        WWS_SUBFAMILY = 22,
        LIGHT_BACKGROUND_PALETTE = 23,
        DARK_BACKGROUND_PALETTE = 24,
        VARIATIONS_POSTSCRIPT_PREFIX = 25,

        _LEN,
    };

    struct Record {
        u16 platformId;
        u16 encodingId;
        u16 languageId;
        NameId nameId;
        u16 length;
        u16 offset;

        bool isUnicode() const {
            return (platformId == 0) or                   // unicode
                   (platformId == 3 and encodingId == 1); // windows
        }
    };

    using StorageOffset = Io::BField<u16be, 4>;

    auto iterRecords() const {
        auto s = begin();
        /* auto version = */ s.nextU16be();
        u16 count = s.nextU16be();
        s.skip(2); // stringOffset

        return Iter{[s, i = 0uz, count] mutable -> Opt<Record> {
            if (i == count)
                return NONE;
            i++;

            Record r;
            r.platformId = s.nextU16be();
            r.encodingId = s.nextU16be();
            r.languageId = s.nextU16be();
            auto nameId = s.nextU16be();
            r.nameId = static_cast<NameId>(nameId);

            r.length = s.nextU16be();
            r.offset = s.nextU16be();

            return r;
        }};
    }

    Record lookupRecord(NameId nameId) const {
        for (auto record : iterRecords()) {
            if (record.nameId == nameId and record.isUnicode())
                return record;
        }

        return {};
    }

    String string(Record const& r) const {
        if (not r.isUnicode())
            return ""s;

        auto s = begin().skip(get<StorageOffset>() + r.offset);
        StringBuilder sb;
        for (usize i = 0; i < r.length / 2; i++) {
            sb.append(s.nextU16be());
        }
        return sb.take();
    }
};

} // namespace Karm::Font::Ttf
