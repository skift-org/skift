#pragma once

#include <karm-base/array.h>
#include <karm-base/slice.h>

namespace Karm::Crypto {

static constexpr usize SHA224_BYTES = 28;
static constexpr usize SHA256_BYTES = 32;
static constexpr usize SHA384_BYTES = 48;
static constexpr usize SHA512_BYTES = 64;

Array<u8, SHA224_BYTES> sha224(Bytes bytes);
Array<u8, SHA256_BYTES> sha256(Bytes bytes);
Array<u8, SHA384_BYTES> sha384(Bytes bytes);
Array<u8, SHA512_BYTES> sha512(Bytes bytes);

} // namespace Karm::Crypto