#include <karm-test/macros.h>
#include <vaev-css/selectors.h>

#include "../parse.h"

namespace Vaev::Css::Tests {

test$("vaev-css-parse-simple-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(parseSelector(""), Style::EmptySelector{});

    expectEq$(parseSelector("html"), htmlSel);
    expectEq$(parseSelector("html "), htmlSel);
    expectEq$(parseSelector(" html"), htmlSel);

    expectEq$(parseSelector(".className"), classSel);
    expectEq$(parseSelector("#idName"), idSel);
    expectEq$(parseSelector("*"), Style::UniversalSelector{});

    return Ok();
}

test$("vaev-css-parse-nfix-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(parseSelector("html,.className"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel}));
    expectEq$(parseSelector("html,.className , \n #idName"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel}));
    expectEq$(parseSelector("html,.className , \n #idName,*"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel, Style::UniversalSelector{}}));

    expectEq$(parseSelector("html.className"), Style::Selector::and_(Vec<Style::Selector>{htmlSel, classSel}));

    return Ok();
}

test$("vaev-css-parse-infix-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(parseSelector("html .className"), Style::Selector::descendant(htmlSel, classSel));
    expectEq$(parseSelector("html>.className"), Style::Selector::child(htmlSel, classSel));
    expectEq$(parseSelector("html > .className"), Style::Selector::child(htmlSel, classSel));
    expectEq$(parseSelector("html > .className #idName"), Style::Selector::descendant(Style::Selector::child(htmlSel, classSel), idSel));
    return Ok();
}

test$("vaev-css-parse-adjacent-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(parseSelector("html +.className"), Style::Selector::adjacent(htmlSel, classSel));
    return Ok();
}

test$("vaev-css-parse-subsequent-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(parseSelector("html~.className"), Style::Selector::subsequent(htmlSel, classSel));
    return Ok();
}

test$("vaev-css-parse-mixed-selectors") {
    // static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    // static auto const classSel = Style::ClassSelector{"className"s};
    // static auto const idSel = Style::IdSelector{"idName"s};

    // expectEq$(parseSelector("html > .className#idName"), Style::Selector::child(htmlSel, Style::Selector::and_(Vec<Style::Selector>{classSel, idSel})));
    // expectEq$(parseSelector("html#idName .className"), Style::Selector::descendant(Style::Selector::child(htmlSel, idSel), classSel));

    return Ok();
}

test$("vaev-css-parse-pseudo-selectors") {
    static auto const htmlSel = Style::TypeSelector{Vaev::Html::HTML};
    static auto const classSel = Style::ClassSelector{"className"s};
    static auto const idSel = Style::IdSelector{"idName"s};

    expectEq$(Style::Pseudo{Style::Pseudo::Type::ROOT}, Style::Pseudo{Style::Pseudo::Type::ROOT});
    expectEq$(parseSelector(":root"), Style::Pseudo{Style::Pseudo::Type::ROOT});
    expectEq$(parseSelector(":root"), Style::Pseudo{Style::Pseudo::make("root")});

    expectEq$(parseSelector("html:hover"), Style::Selector::and_(Vec<Style::Selector>{htmlSel, Style::Pseudo{Style::Pseudo::Type::HOVER}}));

    // this should pass for legacy resons
    // https://www.w3.org/TR/selectors-3/#pseudo-elements
    expectEq$(parseSelector("html:after"), Style::Selector::and_(Vec<Style::Selector>{htmlSel, Style::Pseudo{Style::Pseudo::Type::AFTER}}));

    expectEq$(parseSelector("html::after"), Style::Selector::and_(Vec<Style::Selector>{htmlSel, Style::Pseudo{Style::Pseudo::Type::AFTER}}));
    return Ok();
}

test$("vaev-css-parse-attribute-selectors") {
    static auto const classSel = Style::ClassSelector{"className"s};
    auto const INSENSITIVE = Style::AttributeSelector::INSENSITIVE;
    auto const SENSITIVE = Style::AttributeSelector::SENSITIVE;
    auto const PRESENT = Style::AttributeSelector::Match{Style::AttributeSelector::PRESENT};
    auto const EXACT = Style::AttributeSelector::Match{Style::AttributeSelector::EXACT};
    auto const STR_CONTAIN = Style::AttributeSelector::Match{Style::AttributeSelector::STR_CONTAIN};
    Str name = "type";

    expectEq$(parseSelector(".className[type]"), Style::Selector::and_({classSel, Style::AttributeSelector{name, INSENSITIVE, PRESENT, ""s}}));
    expectEq$(parseSelector(".className[type='text']"), Style::Selector::and_({classSel, Style::AttributeSelector{name, INSENSITIVE, EXACT, "text"s}}));
    expectEq$(parseSelector(".className[ type = 'text' ]"), Style::Selector::and_({classSel, Style::AttributeSelector{name, INSENSITIVE, EXACT, "text"s}}));
    expectEq$(parseSelector(".className[type*='text']"), Style::Selector::and_({classSel, Style::AttributeSelector{name, INSENSITIVE, STR_CONTAIN, "text"s}}));
    expectEq$(parseSelector(".className[type='text' s]"), Style::Selector::and_({classSel, Style::AttributeSelector{name, SENSITIVE, EXACT, "text"s}}));

    return Ok();
}

} // namespace Vaev::Css::Tests
