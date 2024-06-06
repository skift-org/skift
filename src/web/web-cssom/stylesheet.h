#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-mime/url.h>
#include <web-css/lexer.h>
#include <web-media/query.h>
#include <web-select/select.h>

#include "web-css/sst.h"

// SPEC COMPLIANCE : everything related to parent/scope is unsupported as well as everything concerning dynamic modification

namespace Web::CSSOM {

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
    Select::Selector selector;
    // Vec<Css::Sst> selector;

    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct CSSImportRule {
    Vec<CSSStyleDeclaration> declarations;
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct CSSMediaRule {
    Media::Query media;
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
using CSSRule = Union<CSSStyleRule, CSSFontFaceRule, CSSSupportsRule, CSSMediaRule, CSSImportRule>;

// https:// www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<CSSRule> cssRules;
};

} // namespace Web::CSSOM

template <>
struct Karm::Io::Formatter<Web::CSSOM::StyleSheet> {
    Res<usize> format(Io::TextWriter &writer, Web::CSSOM::StyleSheet const &val) {
        usize written = try$(writer.writeRune('{'));
        for (usize i = 0; i < val.cssRules.len(); i++) {
            // written += try$(Io::format(writer, " {#},", val.cssRules[i]));
        }
        written += try$(writer.writeRune('}'));

        return Ok(written);
    }
};

Reflectable$(Web::CSSOM::CSSStyleDeclaration, propertyName, value);

Reflectable$(Web::CSSOM::CSSStyleRule, selector, declarations);
Reflectable$(Web::CSSOM::CSSFontFaceRule, selector, declarations);
Reflectable$(Web::CSSOM::CSSImportRule, declarations);
Reflectable$(Web::CSSOM::CSSMediaRule, media, declarations);
Reflectable$(Web::CSSOM::CSSSupportsRule, declarations);
