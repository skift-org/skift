export module Karm.Core:glob.glob;

import :io.expr;
import :io.sscan;

namespace Karm::Glob {

export bool matchGlob(Io::SScan& glob, Io::SScan& in);

bool _matchWildCard(Io::SScan& glob, Io::SScan& in);

bool _matchGroupe(Io::SScan& glob, Rune curr) {
    bool neg = glob.skip('^');

    while (not glob.ended() and
           not glob.skip(']')) {
        Rune rs = glob.next();
        Rune re = rs;

        if (glob.peek() == '-' and glob.peek(1) != ']') {
            re = glob.peek(1);
            glob.next(2);
        }

        if (re < rs)
            std::swap(rs, re);

        if (rs <= curr and curr <= re) {
            while (not glob.ended() &&
                   not glob.skip(']')) {
                glob.next();
            }

            return !neg;
        }
    }

    return neg;
}

bool _matchWildCard(Io::SScan& glob, Io::SScan& in) {
    for (usize n = in.rem(); n > 0; n--) {
        Io::SScan g = glob, s = in;
        s.next(n);
        if (matchGlob(g, s)) {
            // We found a match, commit the changes
            glob = g, in = s;
            return true;
        }
    }

    return matchGlob(glob, in);
}

// Match a glob pattern against a string.
// The glob pattern can contain the following special characters:
// - '?' matches any single character.
// - '*' matches any sequence of characters.
// - '[' matches any character in the group.
// - '[^' matches any character not in the group.
// - ']' closes the group.
// - '-' specifies a range in the group.
// - '^' negates the group.
export bool matchGlob(Io::SScan& glob, Io::SScan& in) {
    if (glob.remStr() == "*") {
        glob.next();
        in.skip(Re::any());
        return true;
    }

    while (not glob.ended() and !in.ended()) {
        auto op = glob.next();

        switch (op) {
        case '?':
            if (in.ended())
                return false;
            in.next();
            break;

        case '[':
            if (not _matchGroupe(glob, in.next()))
                return false;
            break;

        case '*':
            if (not _matchWildCard(glob, in))
                return false;
            break;

        case '\\':
            if (in.ended())
                return false;

            if (op != in.next())
                return false;

            break;

        default:
            if (in.ended())
                return false;

            if (op != in.next())
                return false;

            break;
        }
    }

    return glob.ended() and in.ended();
}

// Match a string against a glob pattern.
export bool matchGlob(Str glob, Str str) {
    Io::SScan globScan(glob);
    Io::SScan strScan(str);
    return matchGlob(globScan, strScan);
}

} // namespace Karm::Glob
