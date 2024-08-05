#include <karm-test/macros.h>
#include <vaev-style/select.h>

namespace Vaev::Style::Tests {

test$("vaev-style-parse-simple-selectors") {
    expectEq$(
        Selector::parse(""),
        EmptySelector{}
    );

    expectEq$(
        Selector::parse("html"),
        TypeSelector{Html::HTML}
    );

    expectEq$(
        Selector::parse("html "),
        TypeSelector{Html::HTML}
    );

    expectEq$(
        Selector::parse(" html"),
        TypeSelector{Html::HTML}
    );

    expectEq$(
        Selector::parse(".className"),
        ClassSelector{"className"s}
    );

    expectEq$(
        Selector::parse("#idName"),
        IdSelector{"idName"s}
    );

    expectEq$(
        Selector::parse("*"),
        UniversalSelector{}
    );

    return Ok();
}

test$("vaev-style-parse-nfix-selectors") {
    expectEq$(
        Selector::parse("html,.className"),
        Selector::or_({
            TypeSelector{Html::HTML},
            ClassSelector{"className"s},
        })
    );

    expectEq$(
        Selector::parse("html,.className , \n #idName"),
        Selector::or_({
            TypeSelector{Html::HTML},
            ClassSelector{"className"s},
            IdSelector{"idName"s},
        })
    );

    expectEq$(
        Selector::parse("html,.className , \n #idName,*"),
        Selector::or_({
            TypeSelector{Html::HTML},
            ClassSelector{"className"s},
            IdSelector{"idName"s},
            UniversalSelector{},
        })
    );

    expectEq$(
        Selector::parse("html.className"),
        Selector::and_({
            TypeSelector{Html::HTML},
            ClassSelector{"className"s},
        })
    );

    return Ok();
}

test$("vaev-style-parse-infix-selectors") {
    expectEq$(
        Selector::parse("html .className"),
        Selector::descendant(
            TypeSelector{Html::HTML},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        Selector::parse("html>.className"),
        Selector::child(
            TypeSelector{Html::HTML},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        Selector::parse("html > .className"),
        Selector::child(
            TypeSelector{Html::HTML},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        Selector::parse("html > .className #idName"),
        Selector::descendant(
            Selector::child(
                TypeSelector{Html::HTML},
                ClassSelector{"className"s}
            ),
            IdSelector{"idName"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-adjacent-selectors") {
    expectEq$(
        Selector::parse("html +.className"),
        Selector::adjacent(
            TypeSelector{Html::HTML},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-subsequent-selectors") {
    expectEq$(
        Selector::parse("html~.className"),
        Selector::subsequent(
            TypeSelector{Html::HTML},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-mixed-selectors") {
    expectEq$(
        Selector::parse("html > .className#idName"),
        Selector::child(
            TypeSelector{Html::HTML},
            Selector::and_({
                ClassSelector{"className"s},
                IdSelector{"idName"s},
            })
        )
    );

    expectEq$(
        Selector::parse("html#idName .className"),
        Selector::descendant(
            Selector::and_({
                TypeSelector{Html::HTML},
                IdSelector{"idName"s},
            }),
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-pseudo-selectors") {
    expectEq$(
        Pseudo{Pseudo::ROOT},
        Pseudo{Pseudo::ROOT}
    );

    expectEq$(
        Selector::parse(":root"),
        Pseudo{Pseudo::ROOT}
    );

    expectEq$(
        Selector::parse(":root"),
        Pseudo{Pseudo::make("root")}
    );

    expectEq$(
        Selector::parse("html:hover"),
        Selector::and_({
            TypeSelector{Html::HTML},
            Pseudo{Pseudo::HOVER},
        })
    );

    // this should pass for legacy resons
    // https://www.w3.org/TR/selectors-3/#pseudo-elements
    expectEq$(
        Selector::parse("html:after"),
        Selector::and_({
            TypeSelector{Html::HTML},
            Pseudo{Pseudo::AFTER},
        })
    );

    expectEq$(
        Selector::parse("html::after"),
        Selector::and_({
            TypeSelector{Html::HTML},
            Pseudo{Pseudo::AFTER},
        })
    );

    return Ok();
}

test$("vaev-style-parse-attribute-selectors") {
    expectEq$(
        Selector::parse(".className[type]"),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::PRESENT,
                ""s,
            },
        })
    );

    expectEq$(
        Selector::parse(".className[type='text']"),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    expectEq$(
        Selector::parse(".className[ type = 'text' ]"),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    expectEq$(
        Selector::parse(".className[type*='text']"),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::STR_CONTAIN,
                "text"s,
            },
        })
    );

    expectEq$(
        Selector::parse(".className[type='text' s]"),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::SENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    return Ok();
}

} // namespace Vaev::Style::Tests
