#include <karm-gc/heap.h>
#include <karm-test/macros.h>
#include <karm-text/prose.h>
#include <vaev-base/display.h>
#include <vaev-dom/document.h>
#include <vaev-dom/html/parser.h>
#include <vaev-dom/xml/parser.h>
#include <vaev-style/media.h>

import Vaev.Driver;
import Vaev.Layout;

namespace Vaev::Layout::Tests {

static Style::Media const TEST_MEDIA = {
    .type = MediaType::SCREEN,
    .width = 1920_au,
    .height = 1080_au,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = Resolution::fromDpi(96),
    .scan = Scan::PROGRESSIVE,
    .grid = false,
    .update = Update::NONE,
    .overflowBlock = OverflowBlock::NONE,
    .overflowInline = OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = ColorGamut::SRGB,
    .pointer = Pointer::NONE,
    .hover = Hover::NONE,
    .anyPointer = Pointer::FINE,
    .anyHover = Hover::HOVER,

    .prefersReducedMotion = ReducedMotion::REDUCE,
    .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Contrast::LESS,
    .forcedColors = Colors::NONE,
    .prefersColorScheme = ColorScheme::LIGHT,
    .prefersReducedData = ReducedData::REDUCE,

    .deviceWidth = 1920_au,
    .deviceHeight = 1080_au,
    .deviceAspectRatio = 16.0 / 9.0,
};

struct FakeBox;

struct FakeInlineBox {
    Vec<FakeBox> atomicBoxes{};
    Vec<FakeInlineBox> children{};

    struct ComparableInlineBox {
        Vec<ComparableInlineBox> children{};

        ComparableInlineBox& add(ComparableInlineBox&& box) {
            children.pushBack(std::move(box));
            return last(children);
        }

        static ComparableInlineBox fromInlineBox(Layout::InlineBox const& inlineBox) {
            ComparableInlineBox comparableInlineBox;

            Vec<MutCursor<ComparableInlineBox>> stackInlineBoxes = {&comparableInlineBox};
            Vec<Opt<Rc<Text::Prose::Span>>> stackSpans = {NONE};
            for (auto& span : inlineBox.prose->_spans) {
                while (true) {
                    if (span->parent == NONE) {
                        if (last(stackSpans) == NONE)
                            break;

                        (void)stackSpans.popBack();
                        stackInlineBoxes.popBack();
                    } else {
                        if (span->parent->_cell == last(stackSpans)->_cell)
                            break;

                        (void)stackSpans.popBack();
                        stackInlineBoxes.popBack();
                    }
                }

                stackSpans.pushBack(span);
                stackInlineBoxes.pushBack(&last(stackInlineBoxes)->add(ComparableInlineBox{}));
            }

            return comparableInlineBox;
        }
    };

    bool _matches(ComparableInlineBox const& cib) {
        if (children.len() != cib.children.len())
            return false;

        for (usize i = 0; i < children.len(); ++i) {
            if (not children[i]._matches(cib.children[i]))
                return false;
        }

        return true;
    }

    bool matches(InlineBox const& inlineBox);
};

struct FakeBox {
    bool isInternal = false;
    Union<FakeInlineBox, Vec<FakeBox>> content = Vec<FakeBox>{};

    bool matches(Box const& b) {
        if (not(b.style->display.is(Display::Type::DEFAULT) or (isInternal and b.style->display.is(Display::Type::INTERNAL))))
            return false;

        bool fakeBoxStablishesInline = content.is<FakeInlineBox>();
        bool boxStablishesInline = b.content.is<Layout::InlineBox>();

        // logDebug("box: {}, expected: {}", boxStablishesInline, fakeBoxStablishesInline);

        if (boxStablishesInline != fakeBoxStablishesInline)
            return false;

        if (boxStablishesInline) {
            return content.unwrap<FakeInlineBox>().matches(b.content.unwrap<Layout::InlineBox>());
        } else {
            auto& children = content.unwrap<Vec<FakeBox>>();
            // logDebug("box children: {} expected children: {}", b.children().len(), children.len());
            if (children.len() != b.children().len())
                return false;

            for (usize i = 0; i < children.len(); ++i) {
                if (not children[i].matches(b.children()[i]))
                    return false;
            }
            return true;
        }
    }
};

bool FakeInlineBox::matches(InlineBox const& inlineBox) {
    if (atomicBoxes.len() != inlineBox.atomicBoxes.len())
        return false;

    for (usize i = 0; i < atomicBoxes.len(); ++i) {
        if (not atomicBoxes[i].matches(*inlineBox.atomicBoxes[i]))
            return false;
    }

    return _matches(ComparableInlineBox::fromInlineBox(inlineBox));
}

test$("empty-body") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html></body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello, world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = FakeInlineBox{
                // root inline with "hello, world"
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello,<br>brrrrr world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // anon box for hello,
                    .content = FakeInlineBox{
                        // root inline with "hello,"
                    },
                },
                FakeBox{
                    // anon box brrrrr world
                    .content = FakeInlineBox{
                        // root inline with "brrrrr world"
                    },
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("no span, breaking block") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>hello, <div>cruel</div> world</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // anon block
                    .content = FakeInlineBox{
                        // root inline with "hello, "
                    }
                },
                FakeBox{// div block
                        .content = FakeInlineBox{
                            // root inline with "cruel"
                        }
                },
                FakeBox{// anon block
                        .content = FakeInlineBox{
                            // root inline with "world"
                        }
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("span and breaking block 1") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>"
        "<span>hello"
        "<span>cruel"
        "<span>world"
        "<div></div>"
        "</span></span></span>"
        "melancholy"
        "</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // anon div for inlines
                    .content = FakeInlineBox{
                        // root inline box
                        .children = {
                            FakeInlineBox{
                                // span with hello
                                .children = {
                                    FakeInlineBox{
                                        // span with cruel
                                        .children = {
                                            FakeInlineBox{
                                                // span with world
                                            },
                                        }
                                    },
                                }
                            },
                        }
                    },
                },
                FakeBox{
                    // div block
                },
                FakeBox{
                    .content = FakeInlineBox{
                        // root inline box
                        .children = {
                            FakeInlineBox{
                                // span that had hello
                                .children = {
                                    FakeInlineBox{
                                        // span that had cruel
                                        .children = {
                                            FakeInlineBox{
                                                // span that had world
                                            },
                                        },
                                    },
                                },
                                // root inline with have "melancholy"
                            },
                        },
                    },
                },
            },
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("span and breaking block 2") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>"
        "<span>hello"
        "<span>cruel"
        "<span>world"
        "<div></div>"
        "melancholy"
        "</span></span><div>kidding</div></span>"
        "good vibes"
        "</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // anon div for inlines
                    .content = FakeInlineBox{
                        // root inline box
                        .children{
                            FakeInlineBox{
                                // hello
                                .children = {
                                    FakeInlineBox{
                                        // cruel
                                        .children = {
                                            FakeInlineBox{
                                                // world
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
                FakeBox{
                    // misplaced div 1
                },
                FakeBox{
                    // anon div for inline
                    .content = FakeInlineBox{
                        // root inline box
                        .children = {
                            FakeInlineBox{
                                // span with hello
                                .children = {
                                    FakeInlineBox{
                                        // span with cruel
                                        .children = {
                                            FakeInlineBox{
                                                // span with melancholy
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
                FakeBox{
                    // misplaced div 2
                    .content = FakeInlineBox{
                        // root inline box with "kidding"
                    },
                },
                FakeBox{
                    // anon div for inline
                    .content = FakeInlineBox{
                        // root inline box
                        .children = {
                            FakeInlineBox{
                                // span that had "hello"
                            },
                            // root inline with have good vibes in the end
                        },
                    },
                },
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("inline-block") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body>"
        "   A <span>X</span>"
        "   <div id=\"banana\" style=\"display:inline-block\">"
        "       B"
        "       <div id=\"apple\" style=\"display:inline-block\">"
        "           C"
        "       </div>"
        "   </div>"
        "   D"
        "</body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = FakeInlineBox{
                // root inline box with A, strut for bana and D
                .atomicBoxes = {
                    FakeBox{
                        // banana div
                        .content = FakeInlineBox{
                            // root inline with B and strut for apple div
                            .atomicBoxes = {
                                FakeBox{
                                    // apple div
                                    .content = FakeInlineBox{
                                        // root inline box with C
                                    }
                                },
                            },
                        }
                    },
                },
                .children = {
                    FakeInlineBox{
                        // span X
                    },
                }
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("flex-blockify") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());
    Dom::HtmlParser parser{gc, dom};

    parser.write(
        "<html><body><div style=\"display:flex\">"
        "hello"
        "<div style=\"display:inline-block\">hi</div>"
        "goodbye"
        "<div style=\"display:block\">hi</div>"
        "</div></body></html>"
    );

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // flex
                    .content = Vec<FakeBox>{
                        FakeBox{
                            // block for hello
                            .content = FakeInlineBox{}
                        },
                        FakeBox{
                            // blockifyed <div ib>hi</div>, same as a <div>hi</div>
                            .content = FakeInlineBox{}
                        },
                        FakeBox{
                            // block for goodbye
                            .content = FakeInlineBox{}
                        },
                        FakeBox{
                            // block for <div>hi</div>
                            .content = FakeInlineBox{}
                        },
                    }
                }
            }
        };

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

test$("table-fixup-1") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());

    Io::SScan scan{
        "<html><body><table>"
        "wrap me!"
        "<td>wrap me also! and in the same row!</td>"
        "<tr><td>dont wrap me!</td></tr>"
        "<td>wrap mi</td>"
        "<tr>wrap me!</tr>"
        "</table></body></html>"
    };

    auto expectedBodySubtree =
        FakeBox{
            // body
            .content = Vec<FakeBox>{
                FakeBox{
                    // table
                    .content = Vec<FakeBox>{
                        FakeBox{
                            // table-box
                            .isInternal = true,
                            .content = Vec<FakeBox>{
                                FakeBox{
                                    // anon row for "wrap me!" and "wrap me also! and in the same row!"
                                    .isInternal = true,
                                    .content = Vec<FakeBox>{
                                        FakeBox{
                                            // anon cell for "wrap me!"
                                            .isInternal = true,
                                            .content = FakeInlineBox{}
                                        },
                                        FakeBox{
                                            // cell for "wrap me also! and in the same row!"
                                            .isInternal = true,
                                            .content = FakeInlineBox{}
                                        }
                                    }
                                },
                                FakeBox{// row for "dont wrap me!"
                                        .isInternal = true,
                                        .content = Vec<FakeBox>{
                                            FakeBox{.isInternal = true, .content = FakeInlineBox{}},
                                        }
                                },
                                FakeBox{// anon row for "wrap mi"
                                        .isInternal = true,
                                        .content = Vec<FakeBox>{
                                            FakeBox{.isInternal = true, .content = FakeInlineBox{}},
                                        }
                                },
                                FakeBox{// row for "wrap me!"
                                        .isInternal = true,
                                        .content = Vec<FakeBox>{
                                            FakeBox{// anon cell for "wrap me!"
                                                    .isInternal = true,
                                                    .content = FakeInlineBox{}
                                            },
                                        }
                                },
                            }
                        }
                    }
                }
            }
        };

    Dom::XmlParser parser{gc};
    try$(parser.parse(scan, HTML, *dom));

    auto rootBox = Vaev::Driver::render(dom, TEST_MEDIA, Viewport{.small = Vec2Au{100_au, 100_au}}).layout;
    auto const& bodyBox = rootBox->children()[0];

    expect$(expectedBodySubtree.matches(bodyBox));

    return Ok();
}

} // namespace Vaev::Layout::Tests
