#pragma once

#include <karm-base/endian.h>
#include <karm-base/slice.h>
#include <karm-fmt/case.h>
#include <karm-fmt/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/impls.h>
#include <karm-io/traits.h>
#include <karm-logger/logger.h>

namespace Karm::Hash {

template <typename T>
concept HashFunc =
    requires(T t, Bytes bytes) {
        { t.add(bytes) } -> Meta::Same<void>;
        { t.reset() };
        { t.sum() };
        { t.digest() };
        typename T::Digest;
    };

template <typename T>
using HashDigest = typename T::Digest;

template <typename T>
using HashSum = decltype(Meta::declval<T>().sum());

template <usize bits, typename TAG>
struct Digest : public Array<u8, bits / 8> {
};

/* --- Adler32 -------------------------------------------------------------- */

// References:
// https://en.wikipedia.org/wiki/Adler-32

struct Adler32 {
    using Digest = Digest<32, struct Adler32>;

    u32 _sum = 1;

    void add(Bytes bytes) {
        u32 s1 = _sum & 0xffff;
        u32 s2 = _sum >> 16;

        for (usize i = 0; i < bytes.len(); ++i) {
            s1 = (s1 + bytes[i]) % 65521;
            s2 = (s2 + s1) % 65521;
        }

        _sum = (s2 << 16) + s1;
    }

    void reset() {
        _sum = 1;
    }

    u32 sum() const {
        return _sum;
    }

    Digest digest() const {
        return unionCast<Digest>(u32be{_sum});
    }
};

/* --- Crc32 ---------------------------------------------------------------- */

// References:
// https://en.wikipedia.org/wiki/Cyclic_redundancy_check

struct Crc32 {
    using Digest = Digest<32, struct Crc32>;

    u32 _sum = 0xffffffff;
    Array<u32, 256> _table{};

    Crc32(u32 poly = 0xedb88320)
        : _table(makeTable(poly)) {}

    static constexpr Array<u32, 256> makeTable(u32 poly) {
        Array<u32, 256> table;
        for (u32 i = 0; i < 256; ++i) {
            u32 c = i;
            for (u32 j = 0; j < 8; ++j) {
                if (c & 1) {
                    c = (c >> 1) ^ poly;
                } else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }
        return table;
    }

    void add(Bytes bytes) {
        for (usize i = 0; i < bytes.len(); ++i) {
            _sum = _table[(_sum ^ bytes[i]) & 0xff] ^ (_sum >> 8);
        }
    }

    void reset() {
        _sum = 0xffffffff;
    }

    u32 sum() const {
        return ~_sum;
    }

    Digest digest() const {
        return unionCast<Digest, u32be>(sum());
    }
};

static_assert(not Meta::Same<Crc32::Digest, Adler32::Digest>);

/* --- Md5 ------------------------------------------------------------------ */

// References:
// https://en.wikipedia.org/wiki/MD5
// https://github.com/Zunawe/md5-c/tree/main

struct Md5 {
    using Digest = Digest<128, struct Md5>;

    struct Abcd {
        u32 a = 0x67452301;
        u32 b = 0xefcdab89;
        u32 c = 0x98badcfe;
        u32 d = 0x10325476;

        auto operator+=(Abcd const &other) -> Abcd & {
            a += other.a;
            b += other.b;
            c += other.c;
            d += other.d;
            return *this;
        }

        Digest digest() const {
            return {
                u8(a), u8(a >> 8), u8(a >> 16), u8(a >> 24),
                u8(b), u8(b >> 8), u8(b >> 16), u8(b >> 24),
                u8(c), u8(c >> 8), u8(c >> 16), u8(c >> 24),
                u8(d), u8(d >> 8), u8(d >> 16), u8(d >> 24)};
        }
    };

    using Block = Array<u32le, 16>;

    static constexpr Array<u32, 64> S = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    static constexpr Array<u32, 64> K = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    Abcd _sum{};
    usize _len{};

    auto step(Abcd abcd, Block const &input) -> Abcd {
        uint32_t e;

        unsigned int j;

        for (unsigned int i = 0; i < 64; ++i) {
            switch (i / 16) {
            case 0:
                e = (abcd.b & abcd.c) | (~abcd.b & abcd.d);
                j = i;
                break;
            case 1:
                e = (abcd.d & abcd.b) | (~abcd.d & abcd.c);
                j = ((i * 5) + 1) % 16;
                break;
            case 2:
                e = abcd.b ^ abcd.c ^ abcd.d;
                j = ((i * 3) + 5) % 16;
                break;
            default:
                e = abcd.c ^ (abcd.b | ~abcd.d);
                j = (i * 7) % 16;
                break;
            }

            e += abcd.a + K[i] + input[j];
            abcd.a = abcd.d;
            abcd.d = abcd.c;
            abcd.c = abcd.b;
            abcd.b += rol(e, S[i]);
        }

        return abcd;
    }

    usize step(Bytes bytes) {
        Block block;
        auto blockBytes = block.mutBytes();

        // Fill the buffer
        auto n = copy(bytes, mutSub(blockBytes, _len, 64));
        _len += n;

        // If the buffer is full, process it
        if (_len == 64) {
            _sum += step(_sum, block);
            _len = 0;
        }

        return n;
    }

    void add(Bytes bytes) {
        while (bytes.len() > 0) {
            auto n = step(bytes);
            bytes = next(bytes, n);
        }
    }

    void pad() {
        auto block = Block{};
        auto blockBytes = block.mutBytes();
        blockBytes[0] = 0x80;
        blockBytes[15] = _len * 8;
        add(blockBytes);
    }

    void reset() {
        _sum = {};
        _len = 0;
    }

    auto sum() const {
        return _sum;
    }

    auto digest() {
        pad();

        // Add the length
        auto len = _len * 8;
        auto block = Block{};
        auto blockBytes = block.mutBytes();
        blockBytes[14] = len >> 32;
        blockBytes[15] = len;
        add(blockBytes);

        // Return the sum
        return _sum.digest();
    };
};

#define FOR_EACH_HASH(HASH) \
    HASH(ADLER32, Adler32)  \
    HASH(CRC32, Crc32)      \
    HASH(MD5, Md5)

enum struct HashType {
    NIL,
#define ITER(NAME, TYPE) NAME,
    FOR_EACH_HASH(ITER)
#undef ITER
};

using enum HashType;

struct AnyDigest {
    HashType _type;

    Var<
#define ITER(NAME, TYPE) HashDigest<TYPE>,
        FOR_EACH_HASH(ITER)
#undef ITER
            None>
        _buf;

    AnyDigest(None) : _type{NIL}, _buf{NONE} {}

#define ITER(NAME, TYPE) \
    AnyDigest(HashDigest<TYPE> buf) : _type{NAME}, _buf(buf) {}
    FOR_EACH_HASH(ITER)
#undef ITER

    Bytes bytes() const {
        return _buf.visit(Visitor{
            [](None) -> Bytes {
                return {};
            },
            [](auto &buf) -> Bytes {
                return buf.bytes();
            },
        });
    }
};

struct AnyHash {
    using Digest = AnyDigest;

    // clang-format off
    Var<
#define ITER(NAME, TYPE) TYPE,
        FOR_EACH_HASH(ITER)
#undef ITER
    None> _h = NONE;

    using Sum = Var<
#define ITER(NAME, TYPE) HashSum<TYPE>,
        FOR_EACH_HASH(ITER)
#undef ITER
    None>;

    // clang-format on

    AnyHash(HashType type) {
        switch (type) {
        case HashType::ADLER32:
            _h = Adler32{};
            break;

        case HashType::CRC32:
            _h = Crc32{};
            break;

        case HashType::MD5:
            _h = Md5{};
            break;

        default:
            unreachable();
            break;
        }
    }

    template <HashFunc H>
    AnyHash(H h) : _h{h} {}

    void add(Bytes bytes) {
        _h.visit(Visitor{
            [&](None) {
                unreachable();
            },
            [&](auto &h) {
                h.add(bytes);
            },
        });
    }

    void reset() {
        _h.visit(Visitor{
            [&](None) {
                unreachable();
            },
            [&](auto &h) {
                h.reset();
            },
        });
    }

    Sum sum() const {
        return _h.visit(Visitor{
            [&](None) {
                unreachable();
                return None{};
            },
            [&](auto &h) -> Sum {
                return h.sum();
            },
        });
    }

    AnyDigest digest() {
        return _h.visit(Visitor{
            [&](None) {
                unreachable();
                return None{};
            },
            [&](auto &h) -> AnyDigest {
                return h.digest();
            },
        });
    }

    Str name() const {
#define ITER(NAME, TYPE)   \
    [&](TYPE) {            \
        return Str{#NAME}; \
    },

        return _h.visit(Visitor{
            [&](None) {
                unreachable();
                return Str{};
            },
            FOR_EACH_HASH(ITER)});

#undef ITER
    }
};

static inline Res<AnyHash> fromName(Str name) {
#define ITER(NAME, TYPE)            \
    if (eqCi(name, Str{#NAME})) {   \
        return Ok(AnyHash{TYPE{}}); \
    }
    FOR_EACH_HASH(ITER)
#undef ITER

    return Error::invalidInput("unknown hash function");
}

static inline Str name(HashType type) {
#define ITER(NAME, TYPE)          \
    if (type == HashType::NAME) { \
        return Str{#NAME};        \
    }
    FOR_EACH_HASH(ITER)
#undef ITER

    unreachable();
    return {};
}

/* --- Utilities ------------------------------------------------------------ */

template <HashFunc H>
struct SummerReader : public Io::Reader {
    Io::Reader &_reader;
    H _h;

    SummerReader(Io::Reader &reader, H h = {})
        : _reader(reader), _h(h) {}

    Res<usize> read(MutBytes bytes) override {
        auto len = try$(_reader.read(bytes));
        _h.add(sub(bytes, 0, len));
        return Ok(len);
    }

    HashSum<H> sum() const {
        return _h.sum();
    }

    HashDigest<H> digest() {
        return _h.digest();
    }
};

template <HashFunc H>
struct SummerWriter : public Io::Writer {
    Io::Writer &_writer;
    H _h;

    SummerWriter(Io::Writer &writer, H h = {})
        : _writer(writer), _h(h) {}

    Res<usize> write(Bytes bytes) override {
        auto len = try$(_writer.write(bytes));
        _h.add(sub(bytes, 0, len));
        return Ok(len);
    }

    HashSum<H> sum() const {
        return _h.sum;
    }

    HashDigest<H> digest() {
        return _h.digest();
    }
};

template <HashFunc H>
static inline HashSum<H> checksum(Bytes bytes, H h = {}) {
    h.add(bytes);
    return h.sum();
}

template <HashFunc H>
static inline Res<HashSum<H>> checksum(Io::Reader &reader, H h = {}) {
    SummerReader<H> summer{reader, h};
    Io::Sink sink{};
    try$(Io::copy(summer, sink));
    return Ok(summer.sum());
}

template <HashFunc H>
static inline HashDigest<H> digest(Bytes bytes, H h = {}) {
    h.add(bytes);
    return h.digest();
}

template <HashFunc H>
static inline Res<HashDigest<H>> digest(Io::Reader &reader, H h = {}) {
    SummerReader<H> summer{reader, h};
    Io::Sink sink{};
    try$(Io::copy(summer, sink));
    return Ok(summer.digest());
}

} // namespace Karm::Hash

template <>
struct Karm::Fmt::Formatter<Karm::Hash::AnyDigest> {
    Res<usize> format(Io::TextWriter &writer, Karm::Hash::AnyDigest digest) {
        usize writen = 0;
        writen += try$(Fmt::format(writer, "{}:", Fmt::cased(Karm::Hash::name(digest._type), Fmt::Case::LOWER)));
        for (auto byte : digest.bytes()) {
            writen += try$(Fmt::format(writer, "{02x}", byte));
        }
        return Ok(writen);
    }
};
