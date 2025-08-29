#pragma once

import Karm.Core;

#include "macros.h"

using namespace Karm;

namespace Handover {

#ifdef __ck_bits_64__

inline usize KERNEL_BASE = 0xffffffff80000000;
inline usize UPPER_HALF = 0xffff800000000000;
#else

inline usize KERNEL_BASE = 0xc0000000;
inline usize UPPER_HALF = 0xc0000000;

#endif

static constexpr u32 COOLBOOT = 0xc001b001;

#define FOREACH_TAG(TAG)      \
    TAG(FREE, 0)              \
    TAG(MAGIC, COOLBOOT)      \
    TAG(SELF, 0xa24f988d)     \
    TAG(STACK, 0xf65b391b)    \
    TAG(KERNEL, 0xbfc71b20)   \
    TAG(LOADER, 0xf1f80c26)   \
    TAG(FILE, 0xcbc36d3b)     \
    TAG(RSDP, 0x8d3bbb)       \
    TAG(FDT, 0xb628bbc1)      \
    TAG(FB, 0xe2d55685)       \
    TAG(RESERVED, 0xb8841d2d) \
    TAG(END, 0xffffffff)

enum struct Tag : u32 {
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_TAG(ITER)
#undef ITER
};

using enum Tag;

static char const* tagName(Tag tag) {
    switch (tag) {
#define ITER(NAME, VALUE) \
    case Tag::NAME:       \
        return #NAME;
        FOREACH_TAG(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

inline bool shouldMerge(Tag tag) {
    switch (tag) {
    case Tag::FREE:
    case Tag::LOADER:
    case Tag::RESERVED:
        return true;

    default:
        return false;
    }
}

enum struct PixelFormat : u16 {
    RGBX8888 = 0x7451,
    BGRX8888 = 0xd040,
};

struct Record {
    Tag tag;
    u32 flags = 0;
    u64 start = 0;
    u64 size = 0;

    char const* name() const {
        return tagName(tag);
    }

    union {
        struct
        {
            u16 width;
            u16 height;
            u16 pitch;
            PixelFormat format;
        } fb;

        struct
        {
            u32 name;
            u32 meta;
        } file;

        u64 more;
    };

    u64 end() const {
        return start + size;
    }

    bool empty() const {
        return size == 0;
    }

    template <typename R>
    R range() const {
        return R{
            static_cast<usize>(start),
            static_cast<usize>(size),
        };
    }
};

struct Payload {
    u32 magic, agent, size, len;
    Record records[];

    char const* stringAt(u64 offset) const {
        if (offset == 0) {
            return "";
        }
        char const* data = reinterpret_cast<char const*>(this);
        return data + offset;
    }

    char const* agentName() const {
        return stringAt(agent);
    }

    Record const* findTag(Tag tag) const {
        for (auto const& r : *this) {
            if (r.tag == tag) {
                return &r;
            }
        }

        return nullptr;
    }

    Record const* fileByName(char const* name) const {
        for (auto const& r : *this) {
            if (r.tag == Tag::FILE and cstrEq(stringAt(r.file.name), name)) {
                return &r;
            }
        }

        return nullptr;
    }

    Record* begin() {
        return records;
    }

    Record* end() {
        return records + len;
    }

    Record const* begin() const {
        return records;
    }

    Record const* end() const {
        return records + len;
    }

    usize sum(Handover::Tag tag) {
        usize total = 0;
        for (auto const& r : *this) {
            if (r.tag == tag) {
                total += r.size;
            }
        }
        return total;
    }

    Record find(usize size) {
        for (auto& r : *this) {
            if (r.tag == Tag::FREE and r.size >= size) {
                return r;
            }
        }

        return {};
    }

    template <typename R>
    R usableRange() const {
        bool first = true;
        usize start = 0, end = 0;

        for (auto const& r : *this) {
            if (r.tag == Tag::FREE) {
                if (r.start < start or first) {
                    start = r.start;
                    first = false;
                }
                if (r.end() > end) {
                    end = r.end();
                }
            }
        }

        return R{
            start,
            end - start,
        };
    }
};

struct Request {
    Tag tag;
    u32 flags;
    u64 more;

    char const* name() const {
        return tagName(tag);
    }
};

inline constexpr Request requestSelf() {
    return {Tag::SELF, 0, 0};
}

inline constexpr Request requestStack(u64 preferedSize = 64 * 1024) {
    return {Tag::STACK, 0, preferedSize};
}

inline constexpr Request requestKernel() {
    return {Tag::KERNEL, 0, 0};
}

inline constexpr Request requestFiles() {
    return {Tag::FILE, 0, 0};
}

inline constexpr Request requestRsdp() {
    return {Tag::RSDP, 0, 0};
}

inline constexpr Request requestFdt() {
    return {Tag::FDT, 0, 0};
}

inline constexpr Request requestFb(PixelFormat preferedFormat = PixelFormat::BGRX8888) {
    return {Tag::FB, 0, (u64)preferedFormat};
}

inline bool valid(u32 magic, Payload const& payload) {
    if (magic != COOLBOOT) {
        return false;
    }

    if (payload.magic != COOLBOOT) {
        return false;
    }

    return true;
}

static constexpr char const* REQUEST_SECTION = ".handover";

using EntryPoint = void (*)(u64 magic, Payload const* handover);

} // namespace Handover
