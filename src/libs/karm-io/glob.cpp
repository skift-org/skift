#include "glob.h"

namespace Karm::Io {

bool _matchWildCard(SScan &glob, SScan &in);

bool _matchGroupe(SScan &glob, Rune curr) {
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

bool _matchWildCard(SScan &glob, SScan &in) {
    for (usize n = in.rem(); n > 0; n--) {
        SScan g = glob, s = in;
        s.next(n);
        if (matchGlob(g, s)) {
            // We found a match, commit the changes
            glob = g, in = s;
            return true;
        }
    }

    return matchGlob(glob, in);
}

bool matchGlob(SScan &glob, SScan &in) {
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

bool matchGlob(Str glob, Str str) {
    SScan globScan(glob);
    SScan strScan(str);
    return matchGlob(globScan, strScan);
}

} // namespace Karm::Io
