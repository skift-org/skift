#include <karm-test/macros.h>
#include <vaev-css/selectors.h>

#include "vaev-css/mod.h"
namespace Vaev::Css::Tests {
// setup
Style::Selector parse(Str input) {
    logDebug("input: '{}'", input);
    auto lex = Lexer{input};
    auto val = consumeSelector(lex);
    return parseSelector(val);
};

Style::TypeSelector const htmlSel = Style::TypeSelector{TagName::make("html", Vaev::HTML)};
Style::ClassSelector const classSel = Style::ClassSelector{"className"s};
Style::IdSelector const idSel = Style::IdSelector{"idName"s};

test$("vaev-css-parse-simple-selectors") {
    expectEq$(parse(""), Style::EmptySelector{});

    expectEq$(parse("html"), htmlSel);
    expectEq$(parse("html "), htmlSel);
    expectEq$(parse(" html"), htmlSel);

    expectEq$(parse(".className"), classSel);
    expectEq$(parse("#idName"), idSel);
    expectEq$(parse("*"), Style::UniversalSelector{});

    return Ok();
}
test$("vaev-css-parse-nfix-selectors") {
    expectEq$(parse("html,.className"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel}));
    expectEq$(parse("html,.className , \n #idName"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel}));
    expectEq$(parse("html,.className , \n #idName,*"), Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel, Style::UniversalSelector{}}));

    expectEq$(parse("html.className"), Style::Selector::and_(Vec<Style::Selector>{htmlSel, classSel}));

    return Ok();
}
test$("vaev-css-parse-infix-selectors") {
    expectEq$(parse("html .className"), Style::Selector::descendant(htmlSel, classSel));
    expectEq$(parse("html>.className"), Style::Selector::child(htmlSel, classSel));
    expectEq$(parse("html > .className"), Style::Selector::child(htmlSel, classSel));
    expectEq$(parse("html > .className #idName"), Style::Selector::descendant(Style::Selector::child(htmlSel, classSel), idSel));
    return Ok();
}

test$("vaev-css-parse-adjacent-selectors") {
    expectEq$(parse("html +.className"), Style::Selector::adjacent(htmlSel, classSel));
    return Ok();
}

test$("vaev-css-parse-subsequent-selectors") {
    expectEq$(parse("html~.className"), Style::Selector::subsequent(htmlSel, classSel));
    return Ok();
}

test$("vaev-css-parse-mixed-selectors") {
    expectEq$(parse("html > .className#idName"), Style::Selector::child(htmlSel, Style::Selector::and_(Vec<Style::Selector>{classSel, idSel})));
    expectEq$(parse("html#idName .className"), Style::Selector::descendant(Style::Selector::child(htmlSel, idSel), classSel));

    return Ok();
}

} // namespace Vaev::Css::Tests