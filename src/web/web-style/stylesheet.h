#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <web-css/lexer.h>

#include "media.h"
#include "select.h"

namespace Web::Style {

// https://www.w3.org/TR/cssom-1/#the-cssstyledeclaration-interface
struct CSSStyleDeclaration {
    Css::Token propertyName;
    Vec<Css::Token> value;

    CSSStyleDeclaration(Css::Token name)
        : propertyName(name) {
    }

    CSSStyleDeclaration(Css::Token name, Vec<Css::Token> val)
        : propertyName(name), value(val) {
    }

    void getPropertyValue(Str property) {
        logDebug("{}", property);
    };

    void getPropertyPriority(Str property) {
        logDebug("{}", property);
    };
    void setProperty(Str property, Str value, Str priority = "") {
        logDebug("{} : {}, {}", property, value, priority);
    };
    void removeProperty(Str property) {
        logDebug("{}", property);
    };
};

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct CSSStyleRule {
    Selector selector;

    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct CSSImportRule {
    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct CSSMediaRule {
    Query media;
    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct CSSFontFaceRule {
    Vec<Css::Token> selector;
    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/css-conditional-3/#the-csssupportsrule-interface
struct CSSSupportsRule {
    Vec<Css::Token> selector;
    Vec<CSSStyleDeclaration> declarations;
};

// TODO GROUPING RULE

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using CSSRule = Union<
    CSSStyleRule,
    CSSFontFaceRule,
    CSSSupportsRule,
    CSSMediaRule,
    CSSImportRule>;

// https:// www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<CSSRule> rules;
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

Reflectable$(Web::Style::CSSStyleDeclaration, propertyName, value);

Reflectable$(Web::Style::CSSStyleRule, selector, declarations);
Reflectable$(Web::Style::CSSFontFaceRule, selector, declarations);
Reflectable$(Web::Style::CSSImportRule, declarations);
Reflectable$(Web::Style::CSSMediaRule, media, declarations);
Reflectable$(Web::Style::CSSSupportsRule, declarations);
