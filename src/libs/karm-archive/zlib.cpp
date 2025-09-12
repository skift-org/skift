module;

#include <karm-core/macros.h>
#include <karm-logger/logger.h>

export module Karm.Archive:zlib;

import Karm.Core;
import Karm.Debug;
import :flate;

namespace Karm::Archive {

static Debug::Flag debugZlib{"zlib"};

export Res<> zlibDecompress(Io::BitReader& r, Io::Writer& out) {
    u8 cmf = try$(r.readByte());
    logDebugIf(debugZlib, "cmf: {:#02x}", cmf);

    u8 cm = cmf & 0xf;
    if (cm != 8)
        return Error::invalidData("invalid compression method");

    u8 cinfo = (cmf >> 4) & 0xf;
    if (cinfo > 7)
        return Error::invalidData("invalid compression info");

    u8 flg = try$(r.readByte());
    logDebugIf(debugZlib, "flg: {#02x}", flg);

    if ((cmf * 256u + flg) % 31 != 0)
        return Error::invalidData("invalid checksum");

    if ((flg >> 5) & 1)
        return Error::notImplemented("preset dictionary not implemented");

    try$(inflate(r, out));

    u32 adler = try$(r.readBytes<u32>(4));
    logDebugIf(debugZlib, "adler: {:#04x}", adler);
    (void)adler; // TODO: check adler
    return Ok();
}

export Res<> zlibDecompress(Io::Reader& reader, Io::Writer& out) {
    Io::BitReader bits{reader};
    return zlibDecompress(bits, out);
}

export Res<> zlibDecompress(Bytes bytes, Io::Writer& out) {
    Io::BufReader reader{bytes};
    return zlibDecompress(reader, out);
}

export Res<Vec<u8>> zlibDecompress(Bytes bytes) {
    Io::BufferWriter w;
    try$(zlibDecompress(bytes, w));
    return Ok(w.take());
}

} // namespace Karm::Archive