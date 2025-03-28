#include <karm-gc/heap.h>
#include <karm-test/macros.h>
#include <vaev-style/matcher.h>
#include <vaev-style/selector.h>

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Gc::Heap gc;
    Selector sel = ClassSelector{"foo"s};
    auto el = gc.alloc<Dom::Element>(Html::DIV);
    el->classList.add("foo");
    expectNe$(matchSelector(sel, el), NONE);
    return Ok();
}

test$("select-attr-spec-exact") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "test"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesi"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "some test value"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "some testi value"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "test-value"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "test"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesi-value"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "value-test"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "teste"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesitest"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), ""s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "itest"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "testtesi"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), ""s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "value-test-value"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "est-tesi-tes"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), ""s);
        Selector sel = AttributeSelector{
            .name = "id"s,
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
    auto el = gc.alloc<Dom::Element>(Html::DIV);

    {
        el->setAttribute(AttrName::make("id", HTML), "teST"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesT"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "teST"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesT"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "teST"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(AttrName::make("id", HTML), "tesT"s);
        Selector sel = AttributeSelector{
            .name = "id"s,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

} // namespace Vaev::Style::Tests
