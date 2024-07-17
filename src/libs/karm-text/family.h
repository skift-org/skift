#pragma once

#include <karm-logger/logger.h>

#include "book.h"
#include "font.h"

namespace Karm::Text {

struct FontAdjust {
    f64 ascent = 1;
    f64 descent = 1;
    f64 linegap = 1;
    f64 sizeAdjust = 1;

    FontAdjust combine(FontAdjust other) const {
        return {
            .ascent = ascent * other.ascent,
            .descent = descent * other.descent,
            .linegap = linegap * other.linegap,
            .sizeAdjust = sizeAdjust * other.sizeAdjust,
        };
    }
};

struct FontFamily : public Fontface {
    struct Member {
        FontAdjust adjust;
        Strong<Fontface> face;
        Opt<Ranges<Range<Rune>>> ranges;
    };

    FontAdjust _adjust;
    Vec<Member> _members;

    FontFamily(Vec<Member> members) : _members(std::move(members)) {}

    struct Builder : public Meta::NoCopy {
        FontBook const &book;
        Vec<Member> members = {};
        FontAdjust adjust = {};

        Builder(FontBook const &book) : book(book) {}

        Builder &add(FontQuery query) {
            auto face = book.queryClosest(query);

            if (not face) {
                logWarn("failed to find font for query: {}", query);
                return *this;
            }

            members.pushBack({
                .adjust = {},
                .face = *face,
                .ranges = NONE,
            });

            return *this;
        }

        Builder &withAdjust(FontAdjust adjust) {
            last(members).adjust = adjust;
            return *this;
        }

        Builder &adjustAll(FontAdjust adjust) {
            this->adjust.combine(adjust);
            return *this;
        }

        Builder &withRange(Range<Rune> range) {
            auto &member = last(members);
            if (not member.ranges)
                member.ranges = Ranges<Range<Rune>>{};
            last(members).ranges->add(range);
            return *this;
        }

        Strong<FontFamily> bake() {
            return makeStrong<FontFamily>(std::move(members));
        }
    };

    static Builder make(FontBook const &book) {
        return {book};
    }

    FontMetrics metrics() const override {
        FontMetrics metrics = {};

        for (auto &member : _members) {
            auto m = metrics.combine(member.face->metrics());
            auto &a = member.adjust;

            m.ascend *= a.ascent * _adjust.ascent;
            m.descend *= a.descent * _adjust.descent;
            m.linegap *= a.linegap * _adjust.linegap;

            metrics = metrics.combine(m);
        }

        return metrics;
    }

    FontAttrs attrs() const override {
        FontAttrs attrs;

        StringBuilder familyName;
        for (auto &member : _members) {
            if (familyName.len())
                familyName.append(" | "s);
            familyName.append(member.face->attrs().family);
        }

        attrs.family = familyName.take();

        return attrs;
    }

    Glyph glyph(Rune rune) override {
        Glyph res = Glyph::TOFU;

        for (usize i = 0; i < _members.len(); i++) {
            auto &member = _members[i];
            if (member.ranges and not member.ranges->contains(rune)) {
                continue;
            }

            res = member.face->glyph(rune);
            if (res != Glyph::TOFU) {
                res.font = i;
                break;
            }
        }

        if (res == Glyph::TOFU)
            logWarn("failed to find glyph for rune: {:x}", rune);

        return res;
    }

    f64 advance(Glyph glyph) override {
        auto &member = _members[glyph.font];
        auto a = member.face->advance(glyph);
        return a * member.adjust.sizeAdjust * _adjust.sizeAdjust;
    }

    f64 kern(Glyph prev, Glyph curr) override {
        if (prev.font != curr.font)
            return 0;

        auto &member = _members[prev.font];
        auto k = member.face->kern(prev, curr);
        return k * member.adjust.sizeAdjust * _adjust.sizeAdjust;
    }

    void contour(Gfx::Context &g, Glyph glyph) const override {
        auto &member = _members[glyph.font];
        g.scale(_adjust.sizeAdjust * member.adjust.sizeAdjust);
        member.face->contour(g, glyph);
    }
};

} // namespace Karm::Text
