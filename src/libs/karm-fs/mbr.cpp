export module Karm.Fs:mbr;

import Karm.Core;
import :node;

namespace Karm::Fs {

export struct Mbr : Node {
    static constexpr Array<u8, 2> MAGIC = {0x55, 0xAA};
    static constexpr Array<u8, 3> PLACEHOLDER_CHS = {0xfe, 0xff, 0xff};

    struct [[gnu::packed]] Entry {
        u8 attributes;
        Array<u8, 3> firstSector = PLACEHOLDER_CHS;
        u8 partType;
        Array<u8, 3> lastSector = PLACEHOLDER_CHS;
        u32 lbaOffset;
        u32 lbaSize;
    };

    struct Header {
        Array<u8, 446> boostrap = {};
        Array<Entry, 4> entries;
        Array<u8, 2> magic = MAGIC;
    };

    static_assert(sizeof(Header) == 512);

    Rc<Node> _volume;

    static Async::Task<Rc<Mbr>> formatAsync(Rc<Node> volume) {
        (void)volume;
        co_return Error::notImplemented();
    }
};

} // namespace Karm::Fs