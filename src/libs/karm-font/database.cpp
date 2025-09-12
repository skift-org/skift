module;

#include <karm-gfx/font.h>
#include <karm-sys/bundle.h>
#include <karm-sys/dir.h>

export module Karm.Font:database;

import Karm.Core;
import Karm.Ref;
import :loader;

namespace Karm::Font {

export struct Query {
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    Gfx::FontStretch stretch = Gfx::FontStretch::NORMAL;
    Gfx::FontStyle style = Gfx::FontStyle::NORMAL;

    void repr(Io::Emit& e) const {
        e.ln("weight: {}", weight);
        e.ln("stretch: {}", stretch);
        e.ln("style: {}", style);
    }
};

export struct Record {
    Ref::Url url;
    Gfx::FontAttrs attrs;
    Rc<Gfx::Fontface> face;
    Gfx::FontAdjust adjust = {};
};

// MARK: Font Matching ---------------------------------------------------------
// https://www.w3.org/TR/css-fonts-3/#font-matching-algorithm

Str _nextWord(Io::SScan& s) {
    s.eat(Re::space());
    s.eat(Re::word());
    return s.end();
}

Str _peekWord(Io::SScan s) {
    return _nextWord(s);
}

Symbol _commonFamily(Symbol lhs, Symbol rhs) {
    Io::SScan l(lhs.str());
    Io::SScan r(rhs.str());

    l.eat(Re::space());
    r.eat(Re::space());

    l.begin();
    r.begin();

    do {
        Str lword = _peekWord(l);
        Str rword = _peekWord(r);

        if (lword != rword)
            break;

        _nextWord(l);
        _nextWord(r);
    } while (not(l.ended() or l.ended()));

    return Symbol::from(r.end());
}

Symbol _pickFamily(Symbol curr, Symbol best, Symbol desired) {
    if (curr == desired)
        return curr;

    if (best == desired)
        return best;

    auto currPrefix = _commonFamily(curr, desired);
    auto bestPrefix = _commonFamily(best, desired);

    if (currPrefix.str().len() > bestPrefix.str().len())
        return curr;

    return best;
}

Gfx::FontStretch _pickFontStretch(Gfx::FontStretch curr, Gfx::FontStretch best, Gfx::FontStretch desired) {
    if (best == Gfx::FontStretch::NO_MATCH)
        return curr;

    if (curr == desired)
        return curr;

    if (desired <= Gfx::FontStretch::NORMAL) {
        if (best > desired and curr < desired)
            return curr;

        if (best < desired and curr > desired)
            return best;
    } else {
        if (best < desired and curr > desired)
            return curr;

        if (best > desired and curr < desired)
            return best;
    }

    if (curr.delta(desired) < best.delta(desired))
        return curr;

    return best;
}

Gfx::FontWeight _pickFontWeight(Gfx::FontWeight curr, Gfx::FontWeight best, Gfx::FontWeight desired) {
    if (best == Gfx::FontWeight::NO_MATCH)
        return curr;

    if (curr == desired)
        return curr;

    // NOTE: The spec is not clear about what should happen between 400 and 500.
    //       This is a best guess.
    if (desired <= Gfx::FontWeight{450}) {
        if (best > Gfx::FontWeight{500} and curr < Gfx::FontWeight{500})
            return curr;

        if (best < Gfx::FontWeight{500} and curr > Gfx::FontWeight{500})
            return best;
    } else {
        if (best < Gfx::FontWeight{400} and curr > Gfx::FontWeight{400})
            return curr;

        if (best > Gfx::FontWeight{400} and curr < Gfx::FontWeight{400})
            return best;
    }

    if (curr.delta(desired) < best.delta(desired))
        return curr;

    return best;
}

u8 _priority(Gfx::FontStyle desired, Gfx::FontStyle curr) {
    Array<Array<u8, 3>, 3> pri{
        /* NORMAL, OBLIQUE, ITALIC */
        Array<u8, 3>{2, 1, 0}, // NORMAL
        Array<u8, 3>{0, 2, 1}, // OBLIQUE
        Array<u8, 3>{0, 1, 2}, // ITALIC
    };
    return pri[toUnderlyingType(desired)][toUnderlyingType(curr)];
}

Gfx::FontStyle _pickFontStyle(Gfx::FontStyle curr, Gfx::FontStyle best, Gfx::FontStyle desired) {
    if (best == Gfx::FontStyle::NO_MATCH)
        return curr;

    if (_priority(desired, curr) > _priority(desired, best))
        return curr;

    return best;
}

export struct Database {
    Vec<Record> _records;

    // FIXME: these value depend on the correct loading of the bundle
    Map<Symbol, Symbol> _genericFamily = {
        {"serif"_sym, "Noto Serif"_sym},
        {"sans-serif"_sym, "Noto Sans"_sym},
        {"monospace"_sym, "Fira Code"_sym},
        {"cursive"_sym, "Dancing Script"_sym},
        {"fantasy"_sym, "Excalibur"_sym},
        {"system"_sym, "Noto Serif"_sym},
        {"emoji"_sym, "Noto Emoji"_sym},
        {"math"_sym, "Noto Sans Math"_sym},
        {"fangsong"_sym, "Noto"_sym},
    };

    void add(Record record) {
        _records.pushBack(record);
    }

    Res<> load(Ref::Url const& url, Opt<Gfx::FontAttrs> attrs = NONE) {
        auto maybeFace = loadFontface(url);
        if (not maybeFace)
            return maybeFace.none();

        auto face = maybeFace.take();

        add({
            .url = url,
            .attrs = attrs.unwrapOr(face->attrs()),
            .face = face,
        });

        return Ok();
    }

    Res<> loadSystemFonts() {
        auto bundles = try$(Sys::Bundle::installed());
        for (auto& bundle : bundles) {
            auto maybeDir = Sys::Dir::open(bundle.url() / "fonts");
            if (not maybeDir)
                continue;

            auto dir = maybeDir.take();

            for (auto& diren : dir.entries()) {
                if (diren.type != Sys::Type::FILE)
                    continue;

                auto fontUrl = dir.path() / diren.name;
                if (fontUrl.path.suffix() != "ttf")
                    continue;

                if (auto res = load(fontUrl); not res)
                    logWarn("could not load {}: {}", fontUrl, res);
            }
        }

        auto ibmVga = Gfx::Fontface::fallback();

        add({
            .url = ""_url,
            .attrs = ibmVga->attrs(),
            .face = ibmVga,
        });

        return Ok();
    }

    Vec<Symbol> families() const {
        Vec<Symbol> families;
        for (auto& info : _records) {
            bool found = false;
            for (auto& f : families) {
                if (auto prefix = _commonFamily(f, info.attrs.family)) {
                    found = true;
                    f = prefix;
                    break;
                }
            }

            if (not found)
                families.pushBack(info.attrs.family);
        }

        sort(families);
        return families;
    }

    Symbol _resolveGenericFamily(Symbol family) const {
        return _genericFamily.tryGet(family).unwrapOr(family);
    }

    Opt<Rc<Gfx::Fontface>> queryExact(Symbol family, Query query = {}) const {
        auto resolvedFamily = _resolveGenericFamily(family);

        for (auto& info : _records) {

            if (auto& attrs = info.attrs; attrs.family == resolvedFamily and
                                          attrs.weight == query.weight and
                                          attrs.stretch == query.stretch and
                                          attrs.style == query.style)
                return info.face;
        }

        return NONE;
    }

    Opt<Rc<Gfx::Fontface>> queryClosest(Symbol family, Query query = {}) const {
        auto desired = _resolveGenericFamily(family);

        Opt<Rc<Gfx::Fontface>> matchingFace;
        auto matchingFamily = ""_sym;
        auto matchingStretch = Gfx::FontStretch::NO_MATCH;
        auto matchingStyle = Gfx::FontStyle::NO_MATCH;
        auto matchingWeight = Gfx::FontWeight::NO_MATCH;

        for (auto& info : _records) {
            auto const& attrs = info.attrs;

            auto currFamily = matchingFamily;
            auto currStretch = matchingStretch;
            auto currStyle = matchingStyle;
            auto currWeight = matchingWeight;

            currFamily = _pickFamily(attrs.family, matchingFamily, desired);

            if (attrs.family != currFamily)
                continue;

            if (currFamily != matchingFamily) {
                currStretch = Gfx::FontStretch::NO_MATCH;
                currStyle = Gfx::FontStyle::NO_MATCH;
                currWeight = Gfx::FontWeight::NO_MATCH;
            }

            currStretch = _pickFontStretch(attrs.stretch, currStretch, query.stretch);
            if (attrs.stretch != currStretch)
                continue;

            if (currStretch != matchingStretch) {
                currStyle = Gfx::FontStyle::NO_MATCH;
                currWeight = Gfx::FontWeight::NO_MATCH;
            }

            currStyle = _pickFontStyle(attrs.style, currStyle, query.style);
            if (attrs.style != currStyle)
                continue;

            if (currStyle != matchingStyle)
                currWeight = Gfx::FontWeight::NO_MATCH;

            currWeight = _pickFontWeight(attrs.weight, currWeight, query.weight);
            if (attrs.weight != currWeight)
                continue;

            matchingFace = info.face;
            matchingFamily = currFamily;
            matchingStretch = currStretch;
            matchingStyle = currStyle;
            matchingWeight = currWeight;
        }

        return matchingFace;
    }

    Vec<Rc<Gfx::Fontface>> queryFamily(Symbol family) const {
        Vec<Rc<Gfx::Fontface>> res;
        for (auto& info : _records)
            if (_commonFamily(info.attrs.family, family) == family)
                res.pushBack(info.face);

        sort(res, [](auto& lhs, auto& rhs) {
            return lhs->attrs() <=> rhs->attrs();
        });
        return res;
    }
};

export Database const& globalDatabase() {
    static Database db = [] {
        Database res;
        res.loadSystemFonts().unwrap("unable to load system fonts");
        return res;
    }();
    return db;
}

} // namespace Karm::Font
