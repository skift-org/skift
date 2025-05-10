module;

#include <karm-base/rc.h>
#include <karm-math/au.h>
#include <karm-print/page.h>
#include <karm-scene/transform.h>
#include <karm-sys/time.h>
#include <karm-text/book.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Driver:print;

import Vaev.Layout;

namespace Vaev::Driver {

void _paintCornerMargin(Text::FontBook& fontBook, Style::PageComputedStyle& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea area) {
    Layout::Tree tree{
        .root = Layout::buildForPseudoElement(fontBook, pageStyle.area(area)),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto [_, frag] = Layout::layoutCreateFragment(
        tree,
        {
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(frag, stack);
}

void _paintMainMargin(Text::FontBook& fontBook, Style::PageComputedStyle& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas) {
    auto box = Layout::buildForPseudoElement(fontBook, pageStyle.area(mainArea));
    for (auto subArea : subAreas) {
        box.add(Layout::buildForPseudoElement(fontBook, pageStyle.area(subArea)));
    }
    Layout::Tree tree{
        .root = std::move(box),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto [_, frag] = Layout::layoutCreateFragment(
        tree,
        {
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(frag, stack);
}

void _paintMargins(Text::FontBook& fontBook, Style::PageComputedStyle& pageStyle, RectAu pageRect, RectAu pageContent, Scene::Stack& stack) {
    // Compute all corner rects
    auto topLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.topStart(), pageContent.topStart());
    auto topRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.topEnd(), pageContent.topEnd());
    auto bottomLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomStart(), pageContent.bottomStart());
    auto bottomRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomEnd(), pageContent.bottomEnd());

    // Paint corners
    _paintCornerMargin(fontBook, pageStyle, stack, topLeftMarginCornerRect, Style::PageArea::TOP_LEFT_CORNER);
    _paintCornerMargin(fontBook, pageStyle, stack, topRightMarginCornerRect, Style::PageArea::TOP_RIGHT_CORNER);
    _paintCornerMargin(fontBook, pageStyle, stack, bottomLeftMarginCornerRect, Style::PageArea::BOTTOM_LEFT_CORNER);
    _paintCornerMargin(fontBook, pageStyle, stack, bottomRightMarginCornerRect, Style::PageArea::BOTTOM_RIGHT_CORNER);

    // Compute main area rects
    auto topRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.topEnd(), topRightMarginCornerRect.bottomStart());
    auto bottomRect = RectAu::fromTwoPoint(bottomLeftMarginCornerRect.topEnd(), bottomRightMarginCornerRect.bottomStart());
    auto leftRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.bottomEnd(), bottomLeftMarginCornerRect.topStart());
    auto rightRect = RectAu::fromTwoPoint(topRightMarginCornerRect.bottomEnd(), bottomRightMarginCornerRect.topStart());

    // Paint main areas
    _paintMainMargin(fontBook, pageStyle, stack, topRect, Style::PageArea::TOP, {Style::PageArea::TOP_LEFT, Style::PageArea::TOP_CENTER, Style::PageArea::TOP_RIGHT});
    _paintMainMargin(fontBook, pageStyle, stack, bottomRect, Style::PageArea::BOTTOM, {Style::PageArea::BOTTOM_LEFT, Style::PageArea::BOTTOM_CENTER, Style::PageArea::BOTTOM_RIGHT});
    _paintMainMargin(fontBook, pageStyle, stack, leftRect, Style::PageArea::LEFT, {Style::PageArea::LEFT_TOP, Style::PageArea::LEFT_MIDDLE, Style::PageArea::LEFT_BOTTOM});
    _paintMainMargin(fontBook, pageStyle, stack, rightRect, Style::PageArea::RIGHT, {Style::PageArea::RIGHT_TOP, Style::PageArea::RIGHT_MIDDLE, Style::PageArea::RIGHT_BOTTOM});
}

static Style::Media _constructMedia(Print::Settings const& settings) {
    return {
        .type = MediaType::SCREEN,
        .width = Au{settings.paper.width},
        .height = Au{settings.paper.height},
        .aspectRatio = settings.paper.width / (f64)settings.paper.height,
        .orientation = settings.orientation,

        .resolution = Resolution{settings.scale, Resolution::X},
        .scan = Scan::PROGRESSIVE,
        .grid = false,
        .update = Update::FAST,

        .overflowBlock = OverflowBlock::SCROLL,
        .overflowInline = OverflowInline::SCROLL,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = ColorGamut::SRGB,
        .pointer = Pointer::FINE,
        .hover = Hover::HOVER,
        .anyPointer = Pointer::FINE,
        .anyHover = Hover::HOVER,

        .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
        .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
        .prefersContrast = Contrast::NO_PREFERENCE,
        .forcedColors = Colors::NONE,
        .prefersColorScheme = ColorScheme::LIGHT,
        .prefersReducedData = ReducedData::NO_PREFERENCE,

        // NOTE: Deprecated Media Features
        .deviceWidth = Au{settings.paper.width},
        .deviceHeight = Au{settings.paper.height},
        .deviceAspectRatio = settings.paper.width / settings.paper.height,
    };
}

export Generator<Print::Page> print(Gc::Ref<Dom::Document> dom, Print::Settings const& settings) {
    auto media = _constructMedia(settings);

    Text::FontBook fontBook;
    if (not fontBook.loadAll())
        logWarn("not all fonts were properly loaded into fontbook");

    Style::Computer computer{
        media, *dom->styleSheets, fontBook
    };
    computer.loadFontFaces();

    // MARK: Page and Margins --------------------------------------------------

    Style::ComputedStyle initialStyle = Style::ComputedStyle::initial();
    initialStyle.color = Gfx::BLACK;
    initialStyle.setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom->url()))});
    initialStyle.setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom->title()))});
    initialStyle.setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});

    // MARK: Page Content ------------------------------------------------------

    Layout::Tree contentTree = {
        Layout::build(computer, dom),
    };

    Layout::Breakpoint prevBreakpoint{
        .endIdx = 0,
        .advance = Layout::Breakpoint::Advance::WITHOUT_CHILDREN
    };

    usize pageNumber = 0;
    while (true) {
        Style::Page page{
            .name = ""s,
            .number = pageNumber++,
            .blank = false,
        };

        auto pageStyle = computer.computeFor(initialStyle, page);
        RectAu pageRect{
            media.width / Au{media.resolution.toDppx()},
            media.height / Au{media.resolution.toDppx()}
        };

        auto pageSize = pageRect.size().cast<f64>();

        auto pageStack = makeRc<Scene::Stack>();

        InsetsAu pageMargin;

        if (settings.margins == Print::Margins::DEFAULT) {
            Layout::Resolver resolver{};
            pageMargin = {
                resolver.resolve(pageStyle->style->margin->top, pageRect.height),
                resolver.resolve(pageStyle->style->margin->end, pageRect.width),
                resolver.resolve(pageStyle->style->margin->bottom, pageRect.height),
                resolver.resolve(pageStyle->style->margin->start, pageRect.width),
            };
        } else if (settings.margins == Print::Margins::CUSTOM) {
            pageMargin = settings.margins.custom.cast<Au>();
        } else if (settings.margins == Print::Margins::MINIMUM) {
            pageMargin = {};
        }

        RectAu pageContent = pageRect.shrink(pageMargin);

        Layout::Viewport vp{
            .small = pageContent.size(),
        };

        contentTree.viewport = vp;
        contentTree.fc = {pageContent.size()};

        if (settings.headerFooter and settings.margins != Print::Margins::NONE)
            _paintMargins(fontBook, *pageStyle, pageRect, pageContent, *pageStack);

        Layout::Input pageLayoutInput{
            .knownSize = {pageContent.width, NONE},
            .position = pageContent.topStart(),
            .availableSpace = pageContent.size(),
            .containingBlock = pageContent.size(),
        };

        contentTree.fc.enterDiscovery();
        auto outDiscovery = Layout::layout(
            contentTree,
            pageLayoutInput.withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint))
        );

        Layout::Breakpoint currBreakpoint =
            outDiscovery.completelyLaidOut
                ? Layout::Breakpoint::classB(1, false)
                : outDiscovery.breakpoint.unwrap();

        contentTree.fc.leaveDiscovery();
        auto [outReal, fragment] = Layout::layoutCreateFragment(
            contentTree,
            pageLayoutInput
                .withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint, &currBreakpoint))
        );

        Layout::paint(fragment, *pageStack);
        pageStack->prepare();

        co_yield Print::Page(settings.paper, makeRc<Scene::Transform>(pageStack, Math::Trans2f::makeScale(media.resolution.toDppx())));

        if (outReal.completelyLaidOut)
            break;

        prevBreakpoint = std::move(currBreakpoint);
    }
}

} // namespace Vaev::Driver
