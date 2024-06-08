#pragma once

#include <karm-mime/url.h>
#include <web-css/colors.h>

#include "computed.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Web::CSSOM {

// Please keep the props in alphabetical order

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentProp {
    Types::BackgroundAttachment value;

    static Str name() { return "background-attachment"; }

    static auto initial() {
        return Types::BackgroundAttachment::SCROLL;
    }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
struct BackgroundColorProp {
    Gfx::Color value;

    static Str name() { return "background-color"; }

    static auto initial() {
        return Css::TRANSPARENT;
    }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
struct BackgroundImageProp {
    Opt<Mime::Url> value;

    static Str name() { return "background-image"; }

    static auto initial() {
        return NONE;
    }

    void apply(Computed &) const {
        // computed.backgroundImage = value;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
struct BackgroundPositionProp {
    Types::BackgroundPosition value;

    static Str name() { return "background-position"; }

    static Types::BackgroundPosition initial() {
        return {Types::Percent{0}, Types::Percent{0}};
    }

    void apply(Computed &) const {
        // computed.backgroundPosition = value;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
struct BackgroundRepeatProp {
    Types::BackgroundRepeat value;

    static Str name() { return "background-repeat"; }

    static Types::BackgroundRepeat initial() {
        return Types::BackgroundRepeat::REPEAT;
    }

    void apply(Computed &) const {
        // computed.backgroundRepeat = value;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background
struct BackgroundShortand {
    // NOTE: We box the value to avoid bloating the Prop union
    Box<Types::Background> value;

    static Str name() { return "background"; }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
struct BorderCollapseProp {
    Types::BorderCollapse value;

    static Str name() { return "border-collapse"; }

    static auto initial() {
        return Types::BorderCollapse::SEPARATE;
    }

    void apply(Computed &) const {
        // TODO
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderColorProp {
    Gfx::Color value;

    static Str name() { return "border-color"; }

    static auto initial() {
        return Css::BLACK;
    }

    void apply(Computed &) const {
        // TODO
    }
};

struct DisplayProps {
    Types::Display value;

    static Str name() { return "display"; }

    static Types::Display initial() {
        return {Types::Display::FLOW, Types::Display::INLINE};
    }

    void apply(Computed &s) const {
        s.display = value;
    }
};

using _Prop = Union<
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    BackgroundShortand,

    DisplayProps>;

enum struct Important {
    NO,
    YES,
    VERY,
};

struct Prop : public _Prop {
    using _Prop::_Prop;
    Important important = Important::NO;
};

} // namespace Web::CSSOM
