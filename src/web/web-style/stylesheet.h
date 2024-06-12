#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <web-css/lexer.h>

#include "media.h"
#include "props.h"
#include "select.h"

namespace Web::Style {

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct StyleRule {
    Selector selector;
    Vec<Prop> props;
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct ImportRule {
    Mime::Url url;
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct FontFaceRule {
    Vec<Prop> props;
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;
};

// https:// www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<Rule> rules;
};

struct StyleBook {
    Vec<StyleSheet> styleSheets;
};

} // namespace Web::Style

template <>
struct Karm::Io::Formatter<Web::Style::StyleSheet> {
    Res<usize> format(Io::TextWriter &writer, Web::Style::StyleSheet const &val) {
        usize written = try$(writer.writeRune('{'));
        for (usize i = 0; i < val.rules.len(); i++) {
            // written += try$(Io::format(writer, " {#},", val.cssRules[i]));
        }
        written += try$(writer.writeRune('}'));

        return Ok(written);
    }
};

Reflectable$(Web::Style::StyleRule, selector, props);
Reflectable$(Web::Style::FontFaceRule, props);
Reflectable$(Web::Style::ImportRule, url);
Reflectable$(Web::Style::MediaRule, media, rules);
