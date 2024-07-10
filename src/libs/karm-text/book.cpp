#include <karm-logger/logger.h>
#include <karm-pkg/bundle.h>
#include <karm-sys/time.h>

#include "book.h"
#include "loader.h"

namespace Karm::Text {

// MARK: Font loading ----------------------------------------------------------

Res<> FontBook::loadAll() {
    auto start = Sys::now();

    auto bundles = try$(Pkg::installedBundles());
    usize count = 0;
    for (auto &bundle : bundles) {
        auto maybeDir = Sys::Dir::open(bundle.url() / "fonts");
        if (not maybeDir)
            continue;

        auto dir = maybeDir.take();

        for (auto &diren : dir.entries()) {
            if (diren.isDir)
                continue;

            auto fontUrl = dir.path() / diren.name;

            auto maybeFace = loadFontface(fontUrl);
            if (not maybeFace)
                continue;

            auto face = maybeFace.take();

            add({
                .url = fontUrl,
                .attrs = face->attrs(),
                .face = face,
            });
            count++;
        }
    }

    auto elapsed = Sys::now() - start;
    logDebug("Loaded {} fonts in {}", count, elapsed);

    return Ok();
}

// MARK: Font Matching ---------------------------------------------------------
// https://www.w3.org/TR/css-fonts-3/#font-matching-algorithm

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

FaceId FontBook::queryExact(FontQuery query) const {
    auto family = _resolveFamily(query.family);

    for (auto &[id, info] : _faces.iter()) {
        auto &attrs = info.attrs;

        if (attrs.family == family and
            attrs.weight == query.weight and
            attrs.stretch == query.stretch and
            attrs.style == query.style)
            return id;
    }

    return FaceId{0};
}

FaceId FontBook::queryClosest(FontQuery query) const {
    Str family = _resolveFamily(query.family);

    FaceId matchingFace{0};
    auto matchingStretch = FontStretch::NO_MATCH;
    auto matchingStyle = FontStyle::NO_MATCH;
    auto matchingWeight = FontWeight::NO_MATCH;

    for (auto &[id, info] : _faces.iter()) {
        auto const &attrs = info.attrs;

        if (attrs.family != family)
            continue;

        auto currStretch = matchingStretch;
        auto currStyle = matchingStyle;
        auto currWeight = matchingWeight;

        currStretch = _pickFontStretch(attrs.stretch, matchingStretch, query.stretch);
        if (attrs.stretch != currStretch)
            continue;

        if (currStretch != matchingStretch) {
            currStyle = FontStyle::NO_MATCH;
            currWeight = FontWeight::NO_MATCH;
        }

        currStyle = _pickFontStyle(attrs.style, matchingStyle, query.style);
        if (attrs.style != currStyle)
            continue;

        if (currStyle != matchingStyle)
            currWeight = FontWeight::NO_MATCH;

        currWeight = _pickFontWeight(attrs.weight, matchingWeight, query.weight);
        if (attrs.weight != currWeight)
            continue;

        matchingFace = id;
        matchingStretch = currStretch;
        matchingStyle = currStyle;
        matchingWeight = currWeight;
    }

    return matchingFace;
}

Vec<FaceId> FontBook::queryFamily(String family) const {
    Vec<FaceId> ids;
    for (auto &[id, info] : _faces.iter()) {
        if (info.attrs.family == family)
            ids.pushBack(id);
    }
    return ids;
}
} // namespace Karm::Text
