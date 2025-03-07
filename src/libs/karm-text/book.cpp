#include <karm-logger/logger.h>
#include <karm-pkg/bundle.h>
#include <karm-sys/time.h>

#include "book.h"
#include "loader.h"

namespace Karm::Text {

// MARK: Font loading ----------------------------------------------------------

Res<> FontBook::load(Mime::Url const& url, Opt<FontAttrs> attrs) {
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

Res<> FontBook::loadAll() {

    auto bundles = try$(Pkg::installedBundles());
    for (auto& bundle : bundles) {
        auto maybeDir = Sys::Dir::open(bundle.url() / "fonts");
        if (not maybeDir)
            continue;

        auto dir = maybeDir.take();

        for (auto& diren : dir.entries()) {
            if (diren.type != Sys::Type::FILE)
                continue;

            auto fontUrl = dir.path() / diren.name;

            auto res = load(fontUrl);
            if (not res)
                logWarn("could not load {}: {}", fontUrl, res);
        }
    }

    auto ibmVga = Fontface::fallback();

    add({
        .url = ""_url,
        .attrs = ibmVga->attrs(),
        .face = ibmVga,
    });

    return Ok();
}

// MARK: Family Gathering ------------------------------------------------------

static Str _nextWord(Io::SScan& s) {
    s.eat(Re::space());
    s.eat(Re::word());
    return s.end();
}

static Str _peekWord(Io::SScan s) {
    return _nextWord(s);
}

Str commonFamily(Str lhs, Str rhs) {
    Io::SScan l(lhs);
    Io::SScan r(rhs);

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

    return r.end();
}

Vec<String> FontBook::families() const {
    Vec<String> families;
    for (auto& info : _faces) {
        bool found = false;
        for (auto& f : families) {
            auto prefix = commonFamily(f, info.attrs.family);
            if (prefix) {
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

// MARK: Font Matching ---------------------------------------------------------
// https://www.w3.org/TR/css-fonts-3/#font-matching-algorithm

Str _pickFamily(Str curr, Str best, Str desired) {
    if (curr == desired)
        return curr;

    if (best == desired)
        return best;

    auto currPrefix = commonFamily(curr, desired);
    auto bestPrefix = commonFamily(best, desired);

    if (currPrefix.len() > bestPrefix.len())
        return curr;

    return best;
}

FontStretch _pickFontStretch(FontStretch curr, FontStretch best, FontStretch desired) {
    if (best == FontStretch::NO_MATCH)
        return curr;

    if (curr == desired)
        return curr;

    if (desired <= FontStretch::NORMAL) {
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

FontWeight _pickFontWeight(FontWeight curr, FontWeight best, FontWeight desired) {
    if (best == FontWeight::NO_MATCH)
        return curr;

    if (curr == desired)
        return curr;

    // NOTE: The spec is not clear about what should happen between 400 and 500.
    //       This is a best guess.
    if (desired <= FontWeight{450}) {
        if (best > FontWeight{500} and curr < FontWeight{500})
            return curr;

        if (best < FontWeight{500} and curr > FontWeight{500})
            return best;
    } else {
        if (best < FontWeight{400} and curr > FontWeight{400})
            return curr;

        if (best > FontWeight{400} and curr < FontWeight{400})
            return best;
    }

    if (curr.delta(desired) < best.delta(desired))
        return curr;

    return best;
}

u8 _priority(FontStyle desired, FontStyle curr) {
    Array<Array<u8, 3>, 3> pri{
        /* NORMAL, OBLIQUE, ITALIC */
        Array<u8, 3>{2, 1, 0}, // NORMAL
        Array<u8, 3>{0, 2, 1}, // OBLIQUE
        Array<u8, 3>{0, 1, 2}, // ITALIC
    };
    return pri[toUnderlyingType(desired)][toUnderlyingType(curr)];
}

FontStyle _pickFontStyle(FontStyle curr, FontStyle best, FontStyle desired) {
    if (best == FontStyle::NO_MATCH)
        return curr;

    if (_priority(desired, curr) > _priority(desired, best))
        return curr;

    return best;
}

Str FontBook::_resolveFamily(Family const& family) const {
    if (auto gf = family.is<GenericFamily>())
        return _genericFamily[toUnderlyingType(*gf)];
    return family.unwrap<String>();
}

Opt<Rc<Fontface>> FontBook::queryExact(FontQuery query) const {
    auto family = _resolveFamily(query.family);

    for (auto& info : _faces) {
        auto& attrs = info.attrs;

        if (attrs.family == family and
            attrs.weight == query.weight and
            attrs.stretch == query.stretch and
            attrs.style == query.style)
            return info.face;
    }

    return NONE;
}

Opt<Rc<Fontface>> FontBook::queryClosest(FontQuery query) const {
    Str desiredfamily = _resolveFamily(query.family);

    Opt<Rc<Fontface>> matchingFace;
    auto matchingFamily = ""s;
    auto matchingStretch = FontStretch::NO_MATCH;
    auto matchingStyle = FontStyle::NO_MATCH;
    auto matchingWeight = FontWeight::NO_MATCH;

    for (auto& info : _faces) {
        auto const& attrs = info.attrs;

        auto currFamily = matchingFamily;
        auto currStretch = matchingStretch;
        auto currStyle = matchingStyle;
        auto currWeight = matchingWeight;

        currFamily = _pickFamily(attrs.family, matchingFamily, desiredfamily);

        if (attrs.family != currFamily)
            continue;

        if (currFamily != matchingFamily) {
            currStretch = FontStretch::NO_MATCH;
            currStyle = FontStyle::NO_MATCH;
            currWeight = FontWeight::NO_MATCH;
        }

        currStretch = _pickFontStretch(attrs.stretch, currStretch, query.stretch);
        if (attrs.stretch != currStretch)
            continue;

        if (currStretch != matchingStretch) {
            currStyle = FontStyle::NO_MATCH;
            currWeight = FontWeight::NO_MATCH;
        }

        currStyle = _pickFontStyle(attrs.style, currStyle, query.style);
        if (attrs.style != currStyle)
            continue;

        if (currStyle != matchingStyle)
            currWeight = FontWeight::NO_MATCH;

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

Vec<Rc<Fontface>> FontBook::queryFamily(String family) const {
    Vec<Rc<Fontface>> res;
    for (auto& info : _faces)
        if (commonFamily(info.attrs.family, family) == family)
            res.pushBack(info.face);

    sort(res, [](auto& lhs, auto& rhs) {
        return lhs->attrs() <=> rhs->attrs();
    });
    return res;
}

} // namespace Karm::Text
