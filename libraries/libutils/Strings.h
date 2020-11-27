#pragma once

namespace Strings
{

static constexpr auto WHITESPACE = " \n\r\t";
static constexpr auto DIGITS = "0123456789";
static constexpr auto XDIGITS = "0123456789abcdef";
static constexpr auto ALL_XDIGITS = "0123456789abcdefABCDEF";
static constexpr auto LOWERCASE_XDIGITS = "0123456789abcdef";
static constexpr auto UPPERCASE_XDIGITS = "0123456789ABCDEF";
static constexpr auto UTF8BOM = "\xEF\xBB\xBF";

static constexpr auto ALL_ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static constexpr auto LOWERCASE_ALPHA = "abcdefghijklmnopqrstuvwxyz";
static constexpr auto UPPERCASE_ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

} // namespace Strings
