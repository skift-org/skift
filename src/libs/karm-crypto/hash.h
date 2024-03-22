#pragma once

#include <karm-io/fmt.h>

#include "adler32.h"
#include "crc32.h"
#include "md5.h"
#include "sha2.h"

namespace Karm::Crypto {

#define FOR_EACH_HASH(HASH) \
    HASH(ADLER32, Adler32)  \
    HASH(CRC32, Crc32)      \
    HASH(MD5, Md5)          \
    HASH(SHA256, Sha256)

enum struct HashType {
    NIL,
#define ITER(NAME, TYPE) NAME,
    FOR_EACH_HASH(ITER)
#undef ITER
};

using enum HashType;

struct AnyDigest {
    HashType _type;

    Union<
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

    auto operator<=>(AnyDigest const &other) const = default;

    bool operator==(AnyDigest const &other) const = default;
};

struct AnyHash {
    using Digest = AnyDigest;

    // clang-format off
    Union<
#define ITER(NAME, TYPE) TYPE,
        FOR_EACH_HASH(ITER)
#undef ITER
    None> _h = NONE;

    using Sum = Union<
#define ITER(NAME, TYPE) HashSum<TYPE>,
        FOR_EACH_HASH(ITER)
#undef ITER
    None>;

    // clang-format on

    AnyHash(HashType type) {
        switch (type) {
#define ITER(NAME, TYPE) \
    case HashType::NAME: \
        _h = TYPE{};     \
        break;
            FOR_EACH_HASH(ITER)
#undef ITER
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
            FOR_EACH_HASH(ITER)
        });

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

} // namespace Karm::Crypto

template <>
struct Karm::Io::Formatter<Karm::Crypto::AnyDigest> {
    Res<usize> format(Io::TextWriter &writer, Karm::Crypto::AnyDigest digest) {
        usize writen = 0;
        writen += try$(Io::format(writer, "{}:", Io::toLowerCase(Karm::Crypto::name(digest._type))));
        for (auto byte : digest.bytes())
            writen += try$(Io::format(writer, "{02x}", byte));
        return Ok(writen);
    }
};
