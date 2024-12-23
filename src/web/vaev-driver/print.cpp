#include <karm-print/page.h>
#include <karm-scene/transform.h>
#include <karm-sys/time.h>
#include <vaev-layout/builder.h>
#include <vaev-layout/layout.h>
#include <vaev-layout/paint.h>
#include <vaev-layout/tree.h>
#include <vaev-layout/values.h>
#include <vaev-style/page.h>

#include "fetcher.h"
#include "print.h"

namespace Vaev::Driver {

static void _paintMargins(Style::PageComputedStyle &pageStyle, RectPx pageRect, RectPx pageContent, Scene::Stack &stack) {
    // MARK: Top Left Corner ---------------------------------------------------

    auto topLeftMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.topStart(),
        pageContent.topStart()
    );
    Layout::Tree topLeftMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_LEFT_CORNER)),
        .viewport = Layout::Viewport{.small = topLeftMarginCornerRect.size()}
    };

    auto [_, topLeftMarginCornerFrag] = Layout::layoutCreateFragment(
        topLeftMarginCornerTree,
        {
            .knownSize = topLeftMarginCornerRect.size().cast<Opt<Px>>(),
            .position = topLeftMarginCornerRect.topStart(),
            .availableSpace = topLeftMarginCornerRect.size(),
            .containingBlock = topLeftMarginCornerRect.size(),
        }
    );
    Layout::paint(topLeftMarginCornerFrag, stack);

    // MARK: Top Right Corner --------------------------------------------------

    auto topRightMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.topEnd(),
        pageContent.topEnd()
    );
    Layout::Tree topRightMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_RIGHT_CORNER)),
        .viewport = Layout::Viewport{.small = topRightMarginCornerRect.size()}
    };

    auto [_, topRightMarginCornerFrag] = Layout::layoutCreateFragment(
        topRightMarginCornerTree,
        {
            .knownSize = topRightMarginCornerRect.size().cast<Opt<Px>>(),
            .position = topRightMarginCornerRect.topStart(),
            .availableSpace = topRightMarginCornerRect.size(),
            .containingBlock = topRightMarginCornerRect.size(),
        }
    );
    Layout::paint(topRightMarginCornerFrag, stack);

    // MARK: Bottom Left Corner ------------------------------------------------

    auto bottomLeftMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.bottomStart(),
        pageContent.bottomStart()
    );
    Layout::Tree bottomLeftMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_LEFT_CORNER)),
        .viewport = Layout::Viewport{.small = bottomLeftMarginCornerRect.size()}
    };

    auto [_, bottomLeftMarginCornerFrag] = Layout::layoutCreateFragment(
        bottomLeftMarginCornerTree,
        {
            .knownSize = bottomLeftMarginCornerRect.size().cast<Opt<Px>>(),
            .position = bottomLeftMarginCornerRect.topStart(),
            .availableSpace = bottomLeftMarginCornerRect.size(),
            .containingBlock = bottomLeftMarginCornerRect.size(),
        }
    );
    Layout::paint(bottomLeftMarginCornerFrag, stack);

    // MARK: Bottom Right Corner -----------------------------------------------

    auto bottomRightMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.bottomEnd(),
        pageContent.bottomEnd()
    );
    Layout::Tree bottomRightMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_RIGHT_CORNER)),
        .viewport = Layout::Viewport{.small = bottomRightMarginCornerRect.size()}
    };

    auto [_, bottomRightMarginCornerFrag] = Layout::layoutCreateFragment(
        bottomRightMarginCornerTree,
        {
            .knownSize = bottomRightMarginCornerRect.size().cast<Opt<Px>>(),
            .position = bottomRightMarginCornerRect.topStart(),
            .availableSpace = bottomRightMarginCornerRect.size(),
            .containingBlock = bottomRightMarginCornerRect.size(),
        }
    );
    Layout::paint(bottomRightMarginCornerFrag, stack);

    // MARK: Top ---------------------------------------------------------------

    auto topRect = RectPx::fromTwoPoint(
        topLeftMarginCornerRect.topEnd(),
        topRightMarginCornerRect.bottomStart()
    );

    auto topBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_LEFT)));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_CENTER)));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_RIGHT)));

    Layout::Tree topTree{
        .root = std::move(topBox),
        .viewport = Layout::Viewport{.small = topRect.size()}
    };

    auto [_, topFrag] = Layout::layoutCreateFragment(
        topTree,
        {
            .knownSize = topRect.size().cast<Opt<Px>>(),
            .position = topRect.topStart(),
            .availableSpace = topRect.size(),
            .containingBlock = topRect.size(),
        }
    );
    Layout::paint(topFrag, stack);

    // MARK: Bottom ------------------------------------------------------------

    auto bottomRect = RectPx::fromTwoPoint(
        bottomLeftMarginCornerRect.topEnd(),
        bottomRightMarginCornerRect.bottomStart()
    );

    auto bottomBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_LEFT)));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_CENTER)));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_RIGHT)));

    Layout::Tree bottomTree{
        .root = std::move(bottomBox),
        .viewport = Layout::Viewport{.small = bottomRect.size()}
    };

    auto [_, bottomFrag] = Layout::layoutCreateFragment(
        bottomTree,
        {
            .knownSize = bottomRect.size().cast<Opt<Px>>(),
            .position = bottomRect.topStart(),
            .availableSpace = bottomRect.size(),
            .containingBlock = bottomRect.size(),
        }
    );

    Layout::paint(bottomFrag, stack);

    // MARK: Left --------------------------------------------------------------
    auto leftRect = RectPx::fromTwoPoint(
        topLeftMarginCornerRect.bottomEnd(),
        bottomLeftMarginCornerRect.topStart()
    );

    auto leftBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_TOP)));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_MIDDLE)));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_BOTTOM)));

    Layout::Tree leftTree{
        .root = std::move(leftBox),
        .viewport = Layout::Viewport{.small = leftRect.size()}
    };

    auto [_, leftFrag] = Layout::layoutCreateFragment(
        leftTree,
        {
            .knownSize = leftRect.size().cast<Opt<Px>>(),
            .position = leftRect.topStart(),
            .availableSpace = leftRect.size(),
            .containingBlock = leftRect.size(),
        }
    );

    Layout::paint(leftFrag, stack);

    // MARK: Right -------------------------------------------------------------

    auto rightRect = RectPx::fromTwoPoint(
        topRightMarginCornerRect.bottomEnd(),
        bottomRightMarginCornerRect.topStart()
    );

    auto rightBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_TOP)));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_MIDDLE)));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_BOTTOM)));

    Layout::Tree rightTree{
        .root = std::move(rightBox),
        .viewport = Layout::Viewport{.small = rightRect.size()}
    };

    auto [_, rightFrag] = Layout::layoutCreateFragment(
        rightTree,
        {
            .knownSize = rightRect.size().cast<Opt<Px>>(),
            .position = rightRect.topStart(),
            .availableSpace = rightRect.size(),
            .containingBlock = rightRect.size(),
        }
    );

    Layout::paint(rightFrag, stack);
}

static Style::Media _constructMedia(Print::Settings const &settings) {
    return {
        .type = MediaType::SCREEN,
        .width = Px{settings.paper.width},
        .height = Px{settings.paper.height},
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
    };
}

Vec<Print::Page> print(Markup::Document const &dom, Print::Settings const &settings) {
    auto media = _constructMedia(settings);

    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/html.css"_url, Style::Origin::USER_AGENT)
            .take("user agent stylesheet not available")
    );
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/print.css"_url, Style::Origin::USER_AGENT)
            .take("print stylesheet not available")
    );

    fetchStylesheets(dom, stylebook);

    Style::Computer computer{
        media, stylebook
    };

    // MARK: Page and Margins --------------------------------------------------

    Vec<Print::Page> pages;

    Style::Computed initialStyle = Style::Computed::initial();
    initialStyle.color = Gfx::BLACK;
    initialStyle.setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom.url()).unwrap())});
    initialStyle.setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom.title()).unwrap())});
    initialStyle.setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()).unwrap())});

    // MARK: Page Content ------------------------------------------------------

    Layout::Tree contentTree = {
        Layout::build(computer, dom),
    };

    Layout::Breakpoint prevBreakpoint{
        .endIdx = 0,
        .advanceCase = Layout::Breakpoint::ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN
    };
    Layout::Breakpoint currBreakpoint;

    while (true) {
        Layout::Resolver resolver{};
        Style::Page page{.name = ""s, .number = pages.len(), .blank = false};

        auto pageStyle = computer.computeFor(initialStyle, page);
        RectPx pageRect{
            media.width / Px{media.resolution.toDppx()},
            media.height / Px{media.resolution.toDppx()}
        };

        auto pageSize = pageRect.size().cast<f64>();

        auto pageStack = makeStrong<Scene::Stack>();

        InsetsPx pageMargin = {};

        if (settings.margins == Print::Margins::DEFAULT) {
            pageMargin = {
                resolver.resolve(pageStyle->style->margin->top, pageRect.height),
                resolver.resolve(pageStyle->style->margin->end, pageRect.width),
                resolver.resolve(pageStyle->style->margin->bottom, pageRect.height),
                resolver.resolve(pageStyle->style->margin->start, pageRect.width),
            };
        } else if (settings.margins == Print::Margins::CUSTOM) {
            pageMargin = settings.margins.custom.cast<Px>();
        } else if (settings.margins == Print::Margins::MINIMUM) {
        }

        RectPx pageContent = pageRect.shrink(pageMargin);

        Layout::Viewport vp{
            .small = pageContent.size(),
        };

        contentTree.viewport = vp;
        contentTree.fc.currSize = pageContent.size();

        if (settings.headerFooter and settings.margins != Print::Margins::NONE)
            _paintMargins(*pageStyle, pageRect, pageContent, *pageStack);

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

        currBreakpoint = outDiscovery.completelyLaidOut
                             ? Layout::Breakpoint::buildClassB(1, false)
                             : outDiscovery.breakpoint.unwrap();

        contentTree.fc.leaveDiscovery();
        auto [outReal, fragment] = Layout::layoutCreateFragment(
            contentTree,
            pageLayoutInput
                .withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint, &currBreakpoint))
        );

        Layout::paint(fragment, *pageStack);
        pageStack->prepare();

        pages.emplaceBack(settings.paper, makeStrong<Scene::Transform>(pageStack, Math::Trans2f::makeScale(media.resolution.toDppx())));

        if (outReal.completelyLaidOut)
            break;

        std::swap(prevBreakpoint, currBreakpoint);
    }

    return pages;
}

} // namespace Vaev::Driver
