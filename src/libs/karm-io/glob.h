#pragma once

#include <karm-io/sscan.h>

namespace Karm::Io {

// Match a glob pattern against a string.
// The glob pattern can contain the following special characters:
// - '?' matches any single character.
// - '*' matches any sequence of characters.
// - '[' matches any character in the group.
// - '[^' matches any character not in the group.
// - ']' closes the group.
// - '-' specifies a range in the group.
// - '^' negates the group.
bool matchGlob(SScan& glob, SScan& in);

// Match a string against a glob pattern.
bool matchGlob(Str glob, Str str);

} // namespace Karm::Io
