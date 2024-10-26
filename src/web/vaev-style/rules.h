#pragma once

#include "fonts.h"
#include "media.h"
#include "select.h"
#include "styles.h"

namespace Vaev::Style {

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct StyleRule {
    Selector selector;
    Vec<StyleProp> props;

    void repr(Io::Emit &e) const;

    bool match(Markup::Element const &el) const {
        return selector.match(el);
    }

    static StyleRule parse(Css::Sst const &sst);
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct ImportRule {
    Mime::Url url;

    void repr(Io::Emit &e) const;

    static ImportRule parse(Css::Sst const &);
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    void repr(Io::Emit &e) const;

    bool match(Media const &m) const;

    static MediaRule parse(Css::Sst const &sst);
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct FontFaceRule {
    Vec<FontDesc> descs;

    void repr(Io::Emit &e) const;

    static FontFaceRule parse(Css::Sst const &sst);
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit &e) const;

    static Rule parse(Css::Sst const &sst);
};

} // namespace Vaev::Style
