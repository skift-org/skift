#include <karm-test/macros.h>
#include <vaev-css/selectors.h>

#include "vaev-css/mod.h"

namespace Vaev::Css::Tests {
test$("vaev-css-parse-selectors") {
    // setup
    auto testCase = [&](Str input, Style::Selector expected) -> Res<> {
        logDebug("input: '{}'", input);
        auto lex = Lexer{input};
        auto val = consumeDeclarationValue(lex);
        Cursor<Sst> sst{val};
        expectEq$(parseSelector(sst), expected);
        return Ok();
    };

    Style::TypeSelector const htmlSel = Style::TypeSelector{TagName::make("html", Vaev::HTML)};
    Style::ClassSelector const classSel = Style::ClassSelector{"className"s};
    Style::IdSelector const idSel = Style::IdSelector{"idName"s};

    try$(testCase("", Style::EmptySelector{}));
    try$(testCase("html", htmlSel));
    try$(testCase("html ", htmlSel));
    try$(testCase(" html ", htmlSel));
    try$(testCase(".className", classSel));
    try$(testCase("#idName", idSel));
    try$(testCase("*", Style::UniversalSelector{}));

    // NFIX
    try$(testCase("html,.className", Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel})));
    try$(testCase("html,.className , \n #idName", Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel})));
    try$(testCase("html,.className , \n #idName,*", Style::Selector::or_(Vec<Style::Selector>{htmlSel, classSel, idSel, Style::UniversalSelector{}})));

    try$(testCase("html.className", Style::Selector::and_(Vec<Style::Selector>{htmlSel, classSel})));

    // INFIX
    try$(testCase("html .className", Style::Selector::descendant(htmlSel, classSel)));
    try$(testCase("html > .className", Style::Selector::child(htmlSel, classSel)));
    // try$(testCase("html > .className #id", Style::Selector::child(htmlSel, classSel)));

    // MIXED
    // try$(testCase("html .className#id", Style::Selector::child(htmlSel, classSel)));
    // try$(testCase("html#id .className", Style::Selector::child(htmlSel, classSel)));

    return Ok();
}

} // namespace Vaev::Css::Tests