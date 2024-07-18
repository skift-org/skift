#pragma once

#include <karm-text/font.h>
#include <karm-text/run.h>
#include <vaev-paint/text.h>

#include "frag.h"

namespace Vaev::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;
    Strong<Text::Run> _run;

    Run(Strong<Style::Computed> style, Strong<Text::Run> run)
        : Frag(style), _run(run) {
    }

    Type type() const override {
        return TYPE;
    }

    void placeChildren(RectPx bound) override {
        Frag::placeChildren(bound);
    }

    Px computeWidth() override {
        return Px{_run->layout().width};
    }

    Px computeHeight() override {
        return Px{_run->layout().height};
    }

    void makePaintables(Paint::Stack &s) override {
        s.add(makeStrong<Paint::Text>(_borderBox.topStart().cast<f64>(), _run));
    }

    void repr(Io::Emit &e) const override {
        e("({} {})", type(), _borderBox);
    }
};

} // namespace Vaev::Layout
