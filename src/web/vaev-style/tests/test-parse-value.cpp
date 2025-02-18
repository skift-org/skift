#include <karm-test/macros.h>
#include <vaev-css/parser.h>
#include <vaev-style/decls.h>
#include <vaev-style/styles.h>
#include <vaev-style/values.h>

namespace Vaev::Style::Tests {

test$("vaev-css-build-display") {
    auto testCase = [&](Str input, Display expected) -> Res<> {
        auto lex = Css::Lexer{input};
        auto val = consumeDeclarationValue(lex);
        Cursor<Css::Sst> sst{val};
        auto res = parseValue<Display>(sst);
        expect$(res);
        expectEq$(res.unwrap(), expected);
        return Ok();
    };

    // Inside
    try$(testCase("flow", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("flow-root", Display{Display::FLOW_ROOT, Display::BLOCK}));
    try$(testCase("flex", Display{Display::FLEX, Display::BLOCK}));
    try$(testCase("grid", Display{Display::GRID, Display::BLOCK}));
    try$(testCase("ruby", Display{Display::RUBY, Display::BLOCK}));
    try$(testCase("table", Display{Display::TABLE, Display::BLOCK}));
    try$(testCase("math", Display{Display::MATH, Display::BLOCK}));

    // Outside
    try$(testCase("block", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("inline", Display{Display::FLOW, Display::INLINE}));
    try$(testCase("run-in", Display{Display::FLOW, Display::RUN_IN}));

    // Default
    try$(testCase("block flow", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("inline flow", Display{Display::FLOW, Display::INLINE}));
    try$(testCase("run-in flow", Display{Display::FLOW, Display::RUN_IN}));

    // list-item
    try$(testCase("run-in flow list-item", Display{Display::FLOW, Display::RUN_IN}));

    // Internal
    try$(testCase("table-row-group", Display::TABLE_ROW_GROUP));
    try$(testCase("table-header-group", Display::TABLE_HEADER_GROUP));
    try$(testCase("table-footer-group", Display::TABLE_FOOTER_GROUP));
    try$(testCase("table-row", Display::TABLE_ROW));
    try$(testCase("table-cell", Display::TABLE_CELL));
    try$(testCase("table-column-group", Display::TABLE_COLUMN_GROUP));
    try$(testCase("table-column", Display::TABLE_COLUMN));
    try$(testCase("table-caption", Display::TABLE_CAPTION));
    try$(testCase("ruby-base", Display::RUBY_BASE));
    try$(testCase("ruby-text", Display::RUBY_TEXT));
    try$(testCase("ruby-base-container", Display::RUBY_BASE_CONTAINER));
    try$(testCase("ruby-text-container", Display::RUBY_TEXT_CONTAINER));

    // Box
    try$(testCase("contents", Display::CONTENTS));
    try$(testCase("none", Display::NONE));

    // Legacy
    try$(testCase("inline-block", Display{Display::FLOW_ROOT, Display::INLINE}));
    try$(testCase("inline-table", Display{Display::TABLE, Display::INLINE}));
    try$(testCase("inline-flex", Display{Display::FLEX, Display::INLINE}));
    try$(testCase("inline-grid", Display{Display::GRID, Display::INLINE}));

    return Ok();
}

test$("vaev-css-build-margin") {
    auto testCase = [&](Str input, Math::Insets<Width> expected) -> Res<> {
        auto lex = Css::Lexer{input};
        auto val = consumeDeclaration(lex);
        Css::Sst sst{val.unwrap()};

        auto res = parseDeclaration<StyleProp>(sst);
        expect$(res);

        Computed c;
        res.unwrap().apply(c, c);

        expectEq$(Io::format("{}", *c.margin), Io::format("{}", expected));

        return Ok();
    };

    try$(testCase(
        "margin: 30px;",
        Margin{Width{30_au}}
    ));

    try$(testCase(
        "margin: 1px 2px;",
        Margin{
            Width{1_au},
            Width{2_au},
        }
    ));

    try$(testCase(
        "margin: 1px 2px 3px;",
        Margin{
            Width{1_au},
            Width{2_au},
            Width{3_au},
        }
    ));

    try$(testCase(
        "margin: 1px 2px 3px 4px;",
        Margin{
            Width{1_au},
            Width{2_au},
            Width{3_au},
            Width{4_au},
        }
    ));

    return Ok();
}

} // namespace Vaev::Style::Tests
