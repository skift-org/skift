#pragma once

#include <karm-base/rune.h>

namespace Karm::Sys {

#ifdef __ck_sys_encoding_utf8__
using Encoding = Utf8;
#elifdef __ck_sys_encoding_utf16__
using Encoding = Utf16;
#elifdef __ck_sys_encoding_utf32__
using Encoding = Utf32;
#elifdef __ck_sys_encoding_ascii__
using Encoding = Ascii;
#else
#    error "Unknown encoding"
#endif

#ifdef __ck_sys_line_ending_lf__
constexpr char const *LINE_ENDING = "\n";
#    define EMBED_SYS_LINE_ENDING "\n"
#    define EMBED_SYS_LINE_ENDING_L L"\n"
#elifdef __ck_sys_line_ending_crlf__
constexpr char const *LINE_ENDING = "\r\n";
#    define EMBED_SYS_LINE_ENDING "\r\n"
#    define EMBED_SYS_LINE_ENDING_L L"\r\n"
#else
#    error "Unknown line ending"
#endif

#ifdef __ck_sys_path_separator_slash__
constexpr char const *PATH_SEPARATOR = "/";
#    define EMBED_SYS_PATH_SEPARATOR "/"
#    define EMBED_SYS_PATH_SEPARATOR_L L"/"
#elifdef __ck_sys_path_separator_backslash__
constexpr char const *PATH_SEPARATOR = "\\";
#    define EMBED_SYS_PATH_SEPARATOR "\\"
#    define EMBED_SYS_PATH_SEPARATOR_L L"\\"
#else
#    error "Unknown path separator"
#endif

} // namespace Karm::Sys
