#include <karm-test/macros.h>
#include <vaev-style/computer.h>
#include <vaev-style/selector.h>

#include "vaev-style/rules.h"

namespace Vaev::Style::Tests {

test$("test-specificity-selector-list") {
    Selector selector{try$(Selector::parse(".a, .b#x, .c.d.e.f"))};

    auto makeRule = [&]() -> Rule {
        StyleRule rule{
            .selector = selector,
            .props = {}
        };
        return rule;
    };

    Rule rule = makeRule();

    Text::FontBook fontBook;
    StyleBook stylebook;
    Media media;
    Computer computer{media, stylebook, fontBook};

    {
        Dom::Element elZeroMatches{Dom::Element(Html::DIV)};

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elZeroMatches, matchingRules);

        expect$(matchingRules.len() == 0);
    }
    {
        Dom::Element elOneMatch{Dom::Element(Html::DIV)};
        elOneMatch.classList.add("a");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elOneMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(0, 1, 0));
    }
    {
        Dom::Element elOneMatch{Dom::Element(Html::DIV)};
        elOneMatch.classList.add("c");
        elOneMatch.classList.add("d");
        elOneMatch.classList.add("e");
        elOneMatch.classList.add("f");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elOneMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(0, 4, 0));
    }
    {
        Dom::Element elAnotherMatch{Dom::Element(Html::DIV)};
        elAnotherMatch.classList.add("b");
        elAnotherMatch.setAttribute(AttrName::make("id"s, Vaev::HTML), "x"s);

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elAnotherMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(1, 1, 0));
    }
    {
        Dom::Element twoMatches{Dom::Element(Html::DIV)};

        twoMatches.classList.add("b");
        twoMatches.setAttribute(AttrName::make("id"s, Vaev::HTML), "x"s);
        twoMatches.classList.add("a");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, twoMatches, matchingRules);

        expect$(matchingRules[0].v1 == Spec(1, 1, 0));
    }

    return Ok();
}

test$("test-specificity-simple-selector-class") {
    Selector selector{try$(Selector::parse(".a"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 1, 0));

    return Ok();
}

test$("test-specificity-simple-selector-id") {
    Selector selector{try$(Selector::parse("#id"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 0, 0));

    return Ok();
}

test$("test-specificity-simple-selector-type") {
    Selector selector{try$(Selector::parse("div"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 0, 1));

    return Ok();
}

test$("test-specificity-simple-selector-universal") {
    Selector selector{try$(Selector::parse("*"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 0, 0));

    return Ok();
}

test$("test-specificity-simple-selector-attr") {
    Selector selector{try$(Selector::parse("[a=b]"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 1, 0));

    return Ok();
}

test$("test-specificity-simple-selector-pseudo") {
    Selector selector{try$(Selector::parse(":pseu"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 1, 0));

    return Ok();
}

test$("test-specificity-compound-selector-1") {
    Selector selector{try$(Selector::parse(".a.b.c.d"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 4, 0));

    return Ok();
}

test$("test-specificity-compound-selector-2") {
    Selector selector{try$(Selector::parse("#id.a"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 1, 0));

    return Ok();
}

test$("test-specificity-compound-selector-repeated-simple-selector") {
    Selector selector{try$(Selector::parse(".a.a.a"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 3, 0));

    return Ok();
}

test$("test-specificity-compound-selector-one-of-each-simple-1") {
    Selector selector{try$(Selector::parse("div#id.a[x=y]:pseu"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 3, 1));

    return Ok();
}

test$("test-specificity-compound-selector-one-of-each-simple-2") {
    Selector selector{try$(Selector::parse("*#id.a[x=y]:pseu"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 3, 0));

    return Ok();
}

test$("test-specificity-pseudo-not") {
    Selector selector{try$(Selector::parse("button:not([DISABLED])"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(0, 1, 1));

    return Ok();
}

test$("test-specificity-descendant-combinator") {
    Selector selector{try$(Selector::parse("div button .a #x"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 1, 2));

    return Ok();
}

test$("test-specificity-child-combinator") {
    Selector selector{try$(Selector::parse("div > button > .a > #x"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 1, 2));

    return Ok();
}

test$("test-specificity-next-sibling-combinator") {
    Selector selector{try$(Selector::parse("div + button + .a + #x"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 1, 2));

    return Ok();
}

test$("test-specificity-subsequent-sibling-combinator") {
    Selector selector{try$(Selector::parse("div ~ button ~ .a ~ #x"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(1, 1, 2));

    return Ok();
}

test$("test-specificity-mixed-combinators") {
    Selector selector{try$(Selector::parse("div button ~ .a > #x + #y"))};
    Spec specificity{spec(selector)};

    expect$(specificity == Spec(2, 1, 2));

    return Ok();
}

} // namespace Vaev::Style::Tests
