module;

#include <karm-gfx/colors.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-math/trans.h>
#include <karm-sys/time.h>

export module Vaev.Engine:driver.print;

import Karm.Core;
import Karm.Gc;
import Karm.Print;
import Karm.Scene;
import Karm.Font;

import :style;
import :layout;
import :values;
import :dom;
import :css;

using namespace Karm;

namespace Vaev::Driver {

void _paintCornerMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea area) {
    Layout::Tree tree{
        .root = Layout::buildForPseudoElement(pageStyle.area(area)),
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

void _paintMainMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas) {
    auto box = Layout::buildForPseudoElement(pageStyle.area(mainArea));
    for (auto subArea : subAreas) {
        box.add(Layout::buildForPseudoElement(pageStyle.area(subArea)));
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

void _paintMargins(Style::PageSpecifiedValues& pageStyle, RectAu pageRect, RectAu pageContent, Scene::Stack& stack) {
    // Compute all corner rects
    auto topLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.topStart(), pageContent.topStart());
    auto topRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.topEnd(), pageContent.topEnd());
    auto bottomLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomStart(), pageContent.bottomStart());
    auto bottomRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomEnd(), pageContent.bottomEnd());

    // Paint corners
    _paintCornerMargin(pageStyle, stack, topLeftMarginCornerRect, Style::PageArea::TOP_LEFT_CORNER);
    _paintCornerMargin(pageStyle, stack, topRightMarginCornerRect, Style::PageArea::TOP_RIGHT_CORNER);
    _paintCornerMargin(pageStyle, stack, bottomLeftMarginCornerRect, Style::PageArea::BOTTOM_LEFT_CORNER);
    _paintCornerMargin(pageStyle, stack, bottomRightMarginCornerRect, Style::PageArea::BOTTOM_RIGHT_CORNER);

    // Compute main area rects
    auto topRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.topEnd(), topRightMarginCornerRect.bottomStart());
    auto bottomRect = RectAu::fromTwoPoint(bottomLeftMarginCornerRect.topEnd(), bottomRightMarginCornerRect.bottomStart());
    auto leftRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.bottomEnd(), bottomLeftMarginCornerRect.topStart());
    auto rightRect = RectAu::fromTwoPoint(topRightMarginCornerRect.bottomEnd(), bottomRightMarginCornerRect.topStart());

    // Paint main areas
    _paintMainMargin(pageStyle, stack, topRect, Style::PageArea::TOP, {Style::PageArea::TOP_LEFT, Style::PageArea::TOP_CENTER, Style::PageArea::TOP_RIGHT});
    _paintMainMargin(pageStyle, stack, bottomRect, Style::PageArea::BOTTOM, {Style::PageArea::BOTTOM_LEFT, Style::PageArea::BOTTOM_CENTER, Style::PageArea::BOTTOM_RIGHT});
    _paintMainMargin(pageStyle, stack, leftRect, Style::PageArea::LEFT, {Style::PageArea::LEFT_TOP, Style::PageArea::LEFT_MIDDLE, Style::PageArea::LEFT_BOTTOM});
    _paintMainMargin(pageStyle, stack, rightRect, Style::PageArea::RIGHT, {Style::PageArea::RIGHT_TOP, Style::PageArea::RIGHT_MIDDLE, Style::PageArea::RIGHT_BOTTOM});
}

export Generator<Print::Page> print(Gc::Ref<Dom::Document> dom, Print::Settings const& settings) {
    auto media = Style::Media::forPrint(settings);

    Font::Database db;
    if (not db.loadSystemFonts())
        logWarn("not all fonts were properly loaded into database");

    Style::Computer computer{
        media, *dom->styleSheets, db
    };
    computer.build();
    computer.styleDocument(*dom);

    // MARK: Page and Margins --------------------------------------------------

    Style::SpecifiedValues initialStyle = Style::SpecifiedValues::initial();
    initialStyle.color = Gfx::BLACK;
    initialStyle.setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom->url()))});
    initialStyle.setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom->title()))});
    initialStyle.setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});

    // MARK: Page Content ------------------------------------------------------

    Layout::Tree contentTree = {
        Layout::build(dom),
    };
    auto canvasColor = fixupBackgrounds(computer, dom, contentTree);

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

        Layout::Breakpoint currBreakpoint =
            outDiscovery.completelyLaidOut
                ? Layout::Breakpoint::classB(1, false)
                : outDiscovery.breakpoint.unwrap();

        contentTree.fc.leaveDiscovery();
        auto [outFragmentation, fragment] = Layout::layoutCreateFragment(
            contentTree,
            pageLayoutInput
                .withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint, &currBreakpoint))
        );

        Layout::paint(fragment, *pageStack);
        pageStack->prepare();

        co_yield Print::Page(
            settings.paper,
            makeRc<Scene::Clear>(
                makeRc<Scene::Transform>(
                    pageStack,
                    Math::Trans2f::scale(media.resolution.toDppx())
                ),
                canvasColor
            )
        );

        if (outFragmentation.completelyLaidOut)
            break;

        prevBreakpoint = std::move(currBreakpoint);
    }
}

} // namespace Vaev::Driver
