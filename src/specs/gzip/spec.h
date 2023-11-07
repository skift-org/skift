#pragma once

#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

// https://www.ietf.org/rfc/rfc1952.txt
// https://github.com/TechHara/gunzip

namespace Gzip {

struct Header {
    static constexpr u16 MAGIC = 0x8b1f;

    enum Flags {
        TEXT = 1 << 0,
        HCRC = 1 << 1,
        EXTRA = 1 << 2,
        NAME = 1 << 3,
        COMMENT = 1 << 4,
    };

    enum Compression {
        DEFLATE = 8,
    };

    Opt<String> name;

    static Res<Header> parse(Io::BScan &s) {
        Header header;

        if (s.nextU16le() != MAGIC)
            return Error::invalidData("invalid gzip magic");

        auto cm = s.nextU8le();
        if (cm != DEFLATE)
            return Error::invalidData("invalid compression method");

        auto flg = s.nextU8le();

        auto mtime = s.nextU32le();
        (void)mtime; // don't care
        auto xfl = s.nextU8le();
        (void)xfl; // don't care
        auto os = s.nextU8le();
        (void)os; // don't care

        if (flg & EXTRA) {
            auto xlen = s.nextU16le();
            s.skip(xlen);
            logInfo("skipped extra field of size {}", xlen);
        }

        if (flg & NAME) {
            header.name = String{s.nextCStr()};
        }

        if (flg & COMMENT) {
            auto comment = s.nextCStr();
            logInfo("skipped comment field: {}", comment);
        }

        if (flg & HCRC) {
            auto crc16 = s.nextU16le();
            logInfo("skipped header crc16: {}", crc16);
        }

        return Ok(header);
    }
};

struct Footer {
    u32 crc32;
    usize size;

    static Res<Footer> parse(Io::BScan &s) {
        Footer footer;
        footer.crc32 = s.nextU32le();
        footer.size = s.nextU32le();
        return Ok(footer);
    }
};

} // namespace Gzip
