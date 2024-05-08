#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <web-css/lexer.h>

// SPEC COMPLIANCE : everything related to parent/scope is nospec as well as everything concerning dynamic modification

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
        //
        // return "";
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

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
struct CSSRule {
};

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct CSSStyleRule : public CSSRule {
    Vec<Css::Token> selector;
    Vec<CSSStyleDeclaration> declarations;
};

// https:// www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Str type = "text/css";
    Str href;
    Str title = "";
    Vec<CSSStyleRule> cssRules;

    StyleSheet(Str location) {
        href = location;
    }

    void insertRule(Str rule, unsigned long index = 0) {
        logDebug("{} : {}", rule, index);
    };

    void deleteRule(unsigned long index) {
        logDebug("{}", index);
        // TODO remove rule at index in cssRules
    };
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
