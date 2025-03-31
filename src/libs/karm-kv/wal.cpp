module;

#include <karm-crypto/crc32.h>
#include <karm-logger/logger.h>
#include <karm-sys/file.h>

export module Karm.Kv:wal;

import :blob;

namespace Karm::Kv {

export struct Wal {
    struct Record {
        enum struct Type : u8 {
            PUT,
            DEL,
        };

        Type type;
        Blob key;
        Blob value;
    };

    struct [[gnu::packed]] RawHeader {
        static constexpr Array<u8, 8> MAGIC = {
            'K', 'V', 'W', 'A', 'L', 0, 0, 0
        };

        Array<u8, 8> magic;

        Bytes bytes() const {
            return {
                reinterpret_cast<u8 const*>(this),
                sizeof(RawHeader),
            };
        }

        MutBytes mutBytes() {
            return {
                reinterpret_cast<u8*>(this),
                sizeof(RawHeader),
            };
        }
    };

    struct [[gnu::packed]] RawRecord {
        Record::Type type;
        Le<u64> keylen;
        Le<u64> vallen;
        Le<u32> crc;

        Bytes bytes() const {
            return {
                reinterpret_cast<u8 const*>(this),
                sizeof(RawRecord),
            };
        }

        MutBytes mutBytes() {
            return {
                reinterpret_cast<u8*>(this),
                sizeof(RawRecord),
            };
        }

        bool check() {
            auto old = std::exchange(this->crc, 0);
            auto crc = Crypto::crc32(this->bytes());
            this->crc = old;
            return old == crc;
        }
    };

    Sys::File _file;
    RawHeader _header;
    using enum Record::Type;

    static Res<Rc<Wal>> open(Mime::Url const& url) {
        if (Sys::isFile(url)) {
            auto file = try$(Sys::File::openOrCreate(url));
            RawHeader header;
            try$(file.read(header.mutBytes()));
            if (header.magic != RawHeader::MAGIC)
                return Error::invalidData("invalid wal file");

            try$(file.seek(Io::Seek::fromEnd(0)));
            return Ok(makeRc<Wal>(std::move(file), header));
        }

        auto file = try$(Sys::File::openOrCreate(url));
        RawHeader header = {RawHeader::MAGIC};
        try$(file.write(header.bytes()));
        return Ok(makeRc<Wal>(std::move(file)));
    }

    Res<> record(Record::Type type, Bytes key, Bytes value) {
        RawRecord record = {};
        record.type = type;
        record.keylen = key.len();
        record.vallen = value.len();

        record.crc = 0;
        record.crc = Crypto::crc32(record.bytes());

        try$(_file.write(record.bytes()));

        try$(_file.write(key));
        try$(_file.write(value));

        Crypto::Crc32 crc = {};
        crc.update(key);
        crc.update(value);
        try$(_file.write({reinterpret_cast<u8 const*>(&crc), sizeof(crc)}));

        return Ok();
    }

    Generator<Record> iter() {
        // FIXME: Handle errors properly
        _file.seek(Io::Seek::fromBegin(sizeof(RawHeader))).unwrap("could not seek");

        while (true) {
            RawRecord record;
            auto res = _file.read(record.mutBytes());
            if (res.unwrapOr(0) == 0)
                co_return;

            if (not record.check()) {
                panic("invalid crc for record");
                co_return;
            }

            auto key = MutBlob::alloc(record.keylen);
            _file.read(key.mutBytes()).unwrap("could not read key");
            auto value = MutBlob::alloc(record.vallen);
            _file.read(value.mutBytes()).unwrap("could not read value");

            u32le crc;
            _file.read({reinterpret_cast<u8*>(&crc), sizeof(crc)}).unwrap("could not read crc");

            Crypto::Crc32 c;
            c.update(key.bytes());
            c.update(value.bytes());

            if (crc != c.digest()) {
                logError("invalid crc for key/value");
            }

            co_yield Record{
                .type = record.type,
                .key = key,
                .value = value,
            };
        }
    }
};

} // namespace Karm::Kv
