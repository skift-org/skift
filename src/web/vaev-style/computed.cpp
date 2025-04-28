#include "computed.h"

#include "props.h"

namespace Vaev::Style {

ComputedStyle const& ComputedStyle::initial() {
    static ComputedStyle computed = [] {
        ComputedStyle res{};
        StyleProp::any([&]<typename T>() {
            if constexpr (requires { T::initial(); })
                T{}.apply(res);
        });
        return res;
    }();
    return computed;
}

void ComputedStyle::inherit(ComputedStyle const& parent) {
    color = parent.color;
    font = parent.font;
    text = parent.text;
    variables = parent.variables;
    visibility = parent.visibility;
}

void ComputedStyle::repr(Io::Emit& e) const {
    e("(computed");
    e(" color: {}", color);
    e(" opacity: {}", opacity);
    e(" aligns: {}", aligns);
    e(" gaps: {}", gaps);
    e(" backgrounds: {}", backgrounds);
    e(" baseline: {}", baseline);
    e(" borders: {}", borders);
    e(" margin: {}", margin);
    e(" padding: {}", padding);
    e(" boxSizing: {}", boxSizing);
    e(" sizing: {}", sizing);
    e(" overflows: {}", overflows);
    e(" position: {}", position);
    e(" offsets: {}", offsets);
    e(" writingMode: {}", writingMode);
    e(" direction: {}", direction);
    e(" display: {}", display);
    e(" order: {}", order);
    e(" visibility: {}", visibility);
    e(" table: {}", table);
    e(" font: {}", font);
    e(" text: {}", text);
    e(" flex: {}", flex);
    e(" break: {}", break_);
    e(" float: {}", float_);
    e(" clear: {}", clear);
    e(" zIndex: {}", zIndex);
    e(" variables: {}", variables);
    e(")");
}

void ComputedStyle::setCustomProp(Str varName, Css::Content value) {
    variables.cow().put(varName, value);
}

Css::Content ComputedStyle::getCustomProp(Str varName) const {
    auto value = variables->access(varName);
    if (value)
        return *value;
    return {};
}
} // namespace Vaev::Style
