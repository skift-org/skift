#pragma once

#include <karm-base/rune.h>

namespace Embed {

#ifdef __osdk_encoding_utf8__
using Encoding = Utf8;
#elifdef __osdk_encoding_utf16__
using Encoding = Utf16;
#elifdef __osdk_encoding_utf32__
using Encoding = Utf32;
#elifdef __osdk_encoding_ascii__
using Encoding = Ascii;
#endif

} // namespace Embed
