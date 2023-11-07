#pragma once

// This code is based on TechHara/gunzip
// Licensed under BSD-3-Clause
// Copyright (c) 2023, TechHara

// https://github.com/TechHara/gunzip
// https://www.ietf.org/rfc/rfc1951.txt

#include <karm-base/checked.h>
#include <karm-base/cons.h>
#include <karm-base/distinct.h>
#include <karm-base/vec.h>
#include <karm-io/bscan.h>
#include <karm-logger/logger.h>

namespace Deflate {

struct Header {
    enum Type {
        NONE = 0,
        FIXED = 1,
        DYNAMIC = 2,
        RESERVED = 3,
    } type;
    bool final;

    static Res<Header> parse(Io::BScan &s) {
        Header header;
        header.final = s.nextBitbe();
        header.type = (Type)s.nextBitsbe(2);
        return Ok(header);
    }
};

template <Meta::Unsigned T>
constexpr T reverseBits(T val) {
    // https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
    T res = val;
    usize s = sizeof(val) * 8 - 1;
    for (val >>= 1; val; val >>= 1) {
        res <<= 1;
        res |= val & 1;
        s--;
    }
    res <<= s;
    return res;
}

struct Codebook {
    static constexpr usize MAX_BITS = 15;
    static constexpr usize MAX_SYMS = 288;

    Vec<Cons<u32>> tree;
    u32 max;

    static Res<Codebook> make(Slice<u32> lens) {
        if (lens.len() == 0)
            return Error::invalidData("empty codebook");

        if (lens.len() > MAX_SYMS + 1)
            return Error::invalidData("codebook too large");

        Codebook cb{};
        cb.tree.ensure(MAX_SYMS);

        // 1)  Count the number of codes for each code length.  Let
        //     bl_count[N] be the number of codes of length N, N >= 1.
        Array<u32, MAX_BITS + 1> blCount{};
        for (auto l : lens) {
            blCount[l]++;
            cb.tree.pushBack({0, l});
            cb.max = ::max(cb.max, l);
        }

        if (cb.max > MAX_BITS)
            return Error::invalidData("codebook too large");

        // 2) Find the numerical value of the smallest code for each
        //    code length:
        Array<u32, MAX_BITS + 1> next{};
        u32 code = 0;
        blCount[0] = 0;
        for (usize bits = 1; bits <= MAX_BITS; bits++) {
            code = (code + blCount[bits - 1]) << 1;
            next[bits] = code;
        }

        // 3) Assign numerical values to all codes, using consecutive
        //    values for all codes of the same length with the base
        //    values determined at step 2. Codes that are never used
        //    (which have a bit length of zero) must not be assigned a
        //    value.
        for (auto &pair : cb.tree) {
            auto len = pair.cdr;
            if (len != 0)
                pair.car = next[len]++;
        }

        return Ok(cb);
    }

    static Res<Codebook> makeFixedLl() {
        // Table from "3.2.6. Compression with fixed Huffman codes (BTYPE=01)"
        return make(Array{
            // clang-format off
            u32(8), 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8,
            // clang-format on
        });
    }

    static Res<Codebook> makeFixedDist() {
        Array lens{u32(3), 30};
        return make(lens);
    }
};

struct Huffman {
    static constexpr usize NUM_BITS_FIRST_LOOKUP = 9;

    Vec<Cons<u32>> _lookup;
    u32 _primary, _secondary;

    static Huffman make(Codebook const &codebook) {
        Huffman huff;

        auto maxNbits = codebook.max;
        auto nbits = clamp(maxNbits, 0uz, NUM_BITS_FIRST_LOOKUP);

        huff._primary = (1 << nbits) - 1;
        huff._secondary = (1 << (codebook.max - nbits)) - 1;

        huff._lookup.resize(1 << nbits);

        for (u32 sym = 0; sym < codebook.tree.len(); sym++) {
            auto [code, len] = codebook.tree[sym];
            if (len == 0)
                continue;
            code = reverseBits(code);
            code >>= 16 - len;
            if (len <= nbits) {
                auto delta = nbits - len;
                for (usize i = 0; i < (1 << delta); i++)
                    huff._lookup[code | (i << len)] = {sym, len};
            } else {
                auto base = code >> (len - nbits);
                auto off = huff._lookup[base].car;

                if (off == 0) {
                    off = huff._lookup.len();
                    huff._lookup[base] = {off, len};
                    auto newLen = huff._lookup.len() + (1 << (maxNbits - nbits));
                    huff._lookup.resize(newLen);
                }

                auto secondaryLen = len - nbits;
                for (usize i = 0; i < (1 << secondaryLen); i++) {
                    auto base = off + ((code >> nbits) & huff._secondary);
                    huff._lookup[base | (i << secondaryLen)] = {sym, len};
                }
            }
        }

        return huff;
    }

    Res<Cons<u32>> decode(u32 bits) {
        auto [sym, len] = _lookup[bits & _primary];

        if (len == 0)
            return Error::invalidData("huffman code not found");

        if (len <= NUM_BITS_FIRST_LOOKUP)
            return Ok<Cons<u32>>({sym, len});

        return Ok(_lookup[sym + ((bits >> NUM_BITS_FIRST_LOOKUP) & _secondary)]);
    }
};

struct Window : public Io::Writer {
    Buf<Byte> _buf;
    usize _curr;
    usize _max = 1 << 15;

    Bytes buf() {
        return _buf;
    }

    Bytes writeBuf() {
        return next(_buf, _curr);
    }

    void slide(usize off) {
        auto end = _curr + off;
        if (end > _max) {
            auto delta = end - _max;
            copy(sub(_buf, delta, end), mutSub(_buf));
            _curr = _max;
        } else {
            _curr = end;
        }
    }

    usize curr() {
        return _curr;
    }

    Res<usize> write(Bytes bytes) {
        
    }
};

/* --- Lz77 ----------------------------------------------------------------- */

struct Lz77 {

    static constexpr usize EOB = 256;
    static constexpr usize MAX_LEN = 258;

    using Lit = usize;

    struct Eob {};

    struct Dict {
        u16 dist;
        u16 len;
    };

    using Code = Var<Lit, Eob, Dict>;

    static constexpr Array SYM2BITS_LEN = {
        Cons<u32>{0, 0},
        Cons<u32>{0, 3},
        Cons<u32>{0, 4},
        Cons<u32>{0, 5},
        Cons<u32>{0, 6},
        Cons<u32>{0, 7},
        Cons<u32>{0, 8},
        Cons<u32>{0, 9},
        Cons<u32>{0, 10},
        Cons<u32>{1, 11},
        Cons<u32>{1, 13},
        Cons<u32>{1, 15},
        Cons<u32>{1, 17},
        Cons<u32>{2, 19},
        Cons<u32>{2, 23},
        Cons<u32>{2, 27},
        Cons<u32>{2, 31},
        Cons<u32>{3, 35},
        Cons<u32>{3, 43},
        Cons<u32>{3, 51},
        Cons<u32>{3, 59},
        Cons<u32>{4, 67},
        Cons<u32>{4, 83},
        Cons<u32>{4, 99},
        Cons<u32>{4, 115},
        Cons<u32>{5, 131},
        Cons<u32>{5, 163},
        Cons<u32>{5, 195},
        Cons<u32>{5, 227},
        Cons<u32>{0, 258},
    };

    static constexpr Array SYM2BITS_DIST = {
        Cons<u32>{0, 1},
        Cons<u32>{0, 2},
        Cons<u32>{0, 3},
        Cons<u32>{0, 4},
        Cons<u32>{1, 5},
        Cons<u32>{1, 7},
        Cons<u32>{2, 9},
        Cons<u32>{2, 13},
        Cons<u32>{3, 17},
        Cons<u32>{3, 25},
        Cons<u32>{4, 33},
        Cons<u32>{4, 49},
        Cons<u32>{5, 65},
        Cons<u32>{5, 97},
        Cons<u32>{6, 129},
        Cons<u32>{6, 193},
        Cons<u32>{7, 257},
        Cons<u32>{7, 385},
        Cons<u32>{8, 513},
        Cons<u32>{8, 769},
        Cons<u32>{9, 1025},
        Cons<u32>{9, 1537},
        Cons<u32>{10, 2049},
        Cons<u32>{10, 3073},
        Cons<u32>{11, 4097},
        Cons<u32>{11, 6145},
        Cons<u32>{12, 8193},
        Cons<u32>{12, 12289},
        Cons<u32>{13, 16385},
        Cons<u32>{13, 24577},
    };

    static Res<Code> nextCode(Io::BScan &s, Huffman &llHuff, Huffman &distHuff) {
        auto bitcode = s.peekBitsbe(32);
        auto [llSym, llLen] = try$(llHuff.decode(bitcode));
        s.nextBitsbe(llLen);

        if (llSym < EOB)
            return Ok<Code>(Lit{llSym});

        if (llSym == EOB)
            return Ok<Code>(Eob{});

        auto [distSym, distLen] = try$(distHuff.decode(s.peekBitsbe(32)));
        s.nextBitsbe(distLen);
        auto [lenSym, lenLen] = try$(llHuff.decode(s.peekBitsbe(32)));
        s.nextBitsbe(lenLen);

        auto len = SYM2BITS_LEN[lenSym].cdr;
        auto dist = SYM2BITS_DIST[distSym].cdr;

        return Ok<Code>(Dict{(u16)dist, (u16)len});
    };

    struct Decode {
        usize value;
        bool isFull = false;
    };

    static Res<Decode> decode(Io::BScan &s, Huffman &llHuff, Huffman &distHuff, Window &window, usize boundary) {
        auto idx = boundary;
        while (true) {
            if (idx + MAX_LEN > window.curr())
                return Ok(Decode{0, true});

            auto code = try$(nextCode(s, llHuff, distHuff));
            if (auto *lit = code.is<Lit>()) {
                window._buf[idx++] = *lit;
            } else if (auto *dict = code.is<Dict>()) {
                auto [dist, len] = *dict;
                if (dist > idx)
                    return Error::invalidData("invalid distance");
                auto begin = idx - dist;
                while (len > 0) {
                    auto n = ::min(dist, len);
                    copyWithin(mutSub(window._buf), {begin, n}, idx);
                    idx += n;
                    len -= n;
                    dist += n;
                }
            } else if (code.is<Eob>())
                return Ok(Decode{idx});
            else
                unreachable();
        }
    }
};

/* --- Producer ------------------------------------------------------------- */

/* --- Inflate -------------------------------------------------------------- */

struct Inflate {
    Res<usize> _decodeNone(Io::BScan &s, Io::Writer &out) {
        usize len = s.nextU16le();
        usize nlen = s.nextU16le();
        if ((len ^ nlen) != 0xffff)
            return Error::invalidData("invalid len/nlen");
        return out.write(s.nextBytes(len));
    }

    Res<usize> _inflate(Io::BScan &s, Io::Writer &out, Huffman &ll, Huffman &dist) {
    }

    Res<usize> _decodeFixed(Io::BScan &s, Io::Writer &out) {
        static auto ll = Huffman::make(try$(Codebook::makeFixedLl()));
        static auto dist = Huffman::make(try$(Codebook::makeFixedDist()));
    }

    Res<Cons<Huffman>> _parseDynamic(Io::BScan &s) {
        auto hlit = s.nextBitsbe(5) + 257;
        auto hdist = s.nextBitsbe(5) + 1;
        auto hclen = s.nextBitsbe(4) + 4;
        auto clLen = Array<u32, 19>{};

        constexpr Array INDICES{
            // clang-format off
                16,17,18,0,8,7,9,6,
                10,5,11,4,12,3,13,2,14,1,15,
            // clang-format on
        };

        for (auto i : sub(INDICES, 0, hclen)) {
            clLen[i] = s.nextBitsbe(3);
        }

        // The code lengths contain LL codes and Distance codes as a single table
        auto huffman = Huffman::make(
            try$(Codebook::make(clLen)));
        auto numCodes = hlit + hdist;
        Vec<u32> lens;
        lens.ensure(numCodes);
        while (lens.len() < numCodes) {
            auto [code, len] = try$(huffman.decode(s.peekBitsbe(32)));
            s.skipBitsbe(len);

            if (code < 15) {
                lens.pushBack(code);
            } else if (code == 16 and lens.len() > 0) {
                auto len = s.nextBitsbe(2) + 3;
                auto prev = last(lens);
                lens.resize(lens.len() + len, prev);
            } else if (code == 17) {
                auto len = s.nextBitsbe(3) + 3;
                lens.resize(lens.len() + len, 0);
            } else if (code == 18) {
                auto len = s.nextBitsbe(7) + 11;
                lens.resize(lens.len() + len, 0);
            } else
                unreachable();
        }

        if (lens.len() != numCodes)
            return Error::invalidData("invalid code lengths");

        auto ll = try$(Codebook::make(sub(lens, 0, hlit)));
        auto dist = try$(Codebook::make(sub(lens, hlit, hdist)));

        return Ok<Cons<Huffman>>({Huffman::make(ll), Huffman::make(dist)});
    };

    Res<usize> _decodeDynamic(Io::BScan &s, Io::Writer &out) {
        auto [ll, dist] = try$(_parseDynamic(s));
        return _inflate(s, out, ll, dist);
    }

    Res<usize> decode(Io::BScan &s, Io::Writer &out) {
        auto header = try$(Header::parse(s));
        usize written = 0;

        while (not header.final) {
            switch (header.type) {
            case Header::Type::NONE:
                written += try$(_decodeNone(s, out));
                break;

            case Header::Type::FIXED:
                written += try$(_decodeFixed(s, out));
                break;

            case Header::Type::DYNAMIC:
                written += try$(_decodeDynamic(s, out));
                break;

            default:
                return Error::invalidData("reserved compression type");
            }

            header = try$(Header::parse(s));
        }

        return Ok(written);
    }
};

} // namespace Deflate
