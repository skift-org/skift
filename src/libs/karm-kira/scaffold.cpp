module;

#include <karm-app/form-factor.h>
#include <karm-app/inputs.h>
#include <karm-gfx/icon.h>

export module Karm.Kira:scaffold;

import Mdi;
import Karm.Ui;
import :titlebar;
import :toolbar;
import :separator;

namespace Karm::Kira {

export struct Scaffold : Meta::NoCopy {
    Gfx::Icon icon;
    String title;
    TitlebarStyle titlebar = TitlebarStyle::DEFAULT;

    Opt<Ui::Slots> startTools = NONE;
    Opt<Ui::Slots> middleTools = NONE;
    Opt<Ui::Slots> endTools = NONE;
    Opt<Ui::Slot> sidebar = NONE;
    Ui::Slot body;

    Math::Vec2i size = {800, 600};
    bool compact = false;

    struct State {
        bool sidebarOpen = false;
        bool isMobile = false;
    };

    struct ToggleSidebar {};

    using Action = Union<ToggleSidebar>;

    static Ui::Task<Action> reduce(State& s, Action a) {
        if (a.is<ToggleSidebar>()) {
            s.sidebarOpen = !s.sidebarOpen;
        }

        return NONE;
    }

    using Model = Ui::Model<State, Action, reduce>;
};

static Ui::Child _mobileScaffold(Scaffold::State const& s, Scaffold const& scaffold) {
    Ui::Children body;

    if (scaffold.middleTools)
        body.pushBack(toolbar(scaffold.middleTools().unwrap()));

    if (s.sidebarOpen and scaffold.sidebar) {
        body.pushBack(
            (scaffold.sidebar().unwrap()) |
            Ui::grow()
        );
    } else {
        body.pushBack(scaffold.body() | Ui::grow());
    }

    Ui::Children tools;

    if (scaffold.sidebar)
        tools.pushBack(
            Ui::button(
                Scaffold::Model::bind<Scaffold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen
                    ? Mdi::MENU_OPEN
                    : Mdi::MENU
            ) |
            Ui::keyboardShortcut(App::Key::M, App::KeyMod::ALT)
        );

    if (scaffold.startTools)
        tools.pushBack(
            hflow(4, scaffold.startTools().unwrap())
        );

    if (scaffold.startTools and scaffold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scaffold.endTools)
        tools.pushBack(
            hflow(4, scaffold.endTools().unwrap())
        );

    if (tools.len())
        body.pushBack(bottombar(tools));

    return Ui::vflow(body) |
           Ui::pinSize(Math::Vec2i{411, 731}) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

static Ui::Child _desktopScaffold(Scaffold::State const& s, Scaffold const& scaffold) {
    Ui::Children body;

    if (not scaffold.compact)
        body.pushBack(titlebar(scaffold.icon, scaffold.title, scaffold.titlebar));

    Ui::Children tools;

    if (scaffold.compact)
        tools.pushBack(titlebarTitle(scaffold.icon, scaffold.title, true));

    if (scaffold.sidebar)
        tools.pushBack(
            button(
                Scaffold::Model::bind<Scaffold::ToggleSidebar>(),
                Ui::ButtonStyle::subtle(),
                s.sidebarOpen ? Mdi::MENU_OPEN : Mdi::MENU
            ) |
            Ui::keyboardShortcut(App::Key::M, App::KeyMod::ALT)
        );

    if (scaffold.startTools)
        tools.pushBack(
            hflow(4, scaffold.startTools().unwrap())
        );

    if (scaffold.middleTools)
        tools.pushBack(
            hflow(4, scaffold.middleTools().unwrap()) | Ui::grow()
        );

    else if (scaffold.endTools)
        tools.pushBack(Ui::grow(NONE));

    if (scaffold.endTools)
        tools.pushBack(
            hflow(4, scaffold.endTools().unwrap())
        );

    if (scaffold.compact)
        tools.pushBack(titlebarControls(scaffold.titlebar));

    if (tools.len())
        body.pushBack(
            toolbar(tools) | Ui::dragRegion()
        );
    else
        body.pushBack(
            separator()
        );

    if (s.sidebarOpen and scaffold.sidebar) {
        body.pushBack(
            hflow(
                scaffold.sidebar().unwrap(),
                separator(),
                scaffold.body() | Ui::grow()
            ) |
            Ui::grow()
        );
    } else {
        body.pushBack(scaffold.body() | Ui::grow());
    }

    return Ui::vflow(body) |
           Ui::pinSize(scaffold.size) |
           Ui::dialogLayer() |
           Ui::popoverLayer();
}

export Ui::Child scaffold(Scaffold scaffold) {
    auto isMobile = App::formFactor == App::FormFactor::MOBILE;

    Scaffold::State state{
        .sidebarOpen = not isMobile,
        .isMobile = isMobile,
    };

    return Ui::reducer<Scaffold::Model>(state, [scaffold = std::move(scaffold)](Scaffold::State const& state) {
        return state.isMobile
                   ? _mobileScaffold(state, scaffold)
                   : _desktopScaffold(state, scaffold);
    });
}

} // namespace Karm::Kira
