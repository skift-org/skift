#include <karm-test/macros.h>

import Karm.Gc;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Gc::Heap gc;
    Selector sel = ClassSelector{"foo"s};
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);
    el->classList.add("foo");
    expectNe$(matchSelector(sel, el), NONE);
    return Ok();
}

test$("select-attr-spec-exact") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesi"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-contains") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "some test value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "some testi value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-hyphenated") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "test-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesi-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "value-test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-start-with") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "teste"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesitest"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-end-with") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "itest"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "testtesi"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-contain") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "value-test-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "est-tesi-tes"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-case") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("anb-index-match") {
    {
        AnB anb{2, 0};
        expectNot$(anb.match(1));
        expect$(anb.match(2));
        expectNot$(anb.match(3));
        expect$(anb.match(4));
    }
    {
        AnB anb{2, 1};
        expect$(anb.match(1));
        expectNot$(anb.match(2));
        expect$(anb.match(3));
        expectNot$(anb.match(4));
    }
    {
        AnB anb{3, -1};
        // n = 0: -1 (won't match)
        // n = 1: 2
        // n = 2: 5
        // n = 3: 8
        // n ...:
        for (usize i = 1; i <= 10; i++) {
            if (i == 2 or i == 5 or i == 8) {
                expect$(anb.match(i));
            } else {
                expectNot$(anb.match(i));
            }
        }
    }
    {
        AnB anb{-3, 10};
        // n = 0: 10
        // n = 1: 7
        // n = 2: 4
        // n = 3: 1
        // n ...: won't match
        for (usize i = 1; i <= 20; i++) {
            if (i == 1 or i == 4 or i == 7 or i == 10) {
                expect$(anb.match(i));
            } else {
                expectNot$(anb.match(i));
            }
        }
    }
    {
        AnB anb{1, 1};
        for (usize i = 1; i <= 20; i++) {
            expect$(anb.match(i));
        }
    }
    {
        AnB anb{1, 2};
        expectNot$(anb.match(1));
        for (usize i = 2; i <= 20; i++) {
            expect$(anb.match(i));
        }
    }
    {
        AnB anb{0, 0};
        for (usize i = 1; i <= 20; i++) {
            expectNot$(anb.match(i));
        }
    }
    {
        AnB anb{0, 21};
        for (usize i = 1; i <= 20; i++) {
            expectNot$(anb.match(i));
        }
        expect$(anb.match(21));
    }

    return Ok();
}

} // namespace Vaev::Style::Tests
