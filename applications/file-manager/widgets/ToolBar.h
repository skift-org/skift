#pragma once

#include <libwidget/widgets/Panel.h>
#include <libwidget/widgets/Separator.h>
#include <libwidget/widgets/Toolbar.h>

#include "file-manager/model/Navigation.h"
#include "file-manager/widgets/Breadcrumb.h"

namespace file_manager
{

class ToolBar: public Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    Widget *_go_backward;
    Widget *_go_foreward;
    Widget *_go_up;
    Widget *_go_home;

    Widget *_breadcrumb;

    Widget *_refresh;
    Widget *_open_terminal;

    OwnPtr<Observer<Navigation>> _observer;

public:
    ToolBar(Widget *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks) :
        Panel(parent),
        _navigation(navigation),
        _bookmarks(bookmarks)
    {
        layout(HFLOW(4));
        insets(Insets(4, 4));
        max_height(38);
        min_height(38);

        _go_backward = toolbar_icon_create(this, Icon::get("arrow-left"));

        _go_backward->on(Event::ACTION, [this](auto) {
            _navigation->go_backward();
        });

        _go_foreward = toolbar_icon_create(this, Icon::get("arrow-right"));

        _go_foreward->on(Event::ACTION, [this](auto) {
            _navigation->go_forward();
        });

        _go_up = toolbar_icon_create(this, Icon::get("arrow-up"));

        _go_up->on(Event::ACTION, [this](auto) {
            _navigation->go_up();
        });

        _go_home = toolbar_icon_create(this, Icon::get("home"));

        _go_home->on(Event::ACTION, [this](auto) {
            _navigation->go_home();
        });

        new Separator(this);

        _breadcrumb = new Breadcrumb(this, _navigation, _bookmarks);
        _breadcrumb->attributes(LAYOUT_FILL);

        new Separator(this);

        _refresh = toolbar_icon_create(this, Icon::get("refresh"));

        _refresh->on(Event::ACTION, [this](auto) {
            _navigation->refresh();
        });

        Widget *terminal_button = toolbar_icon_create(this, Icon::get("console"));

        terminal_button->on(Event::ACTION, [](auto) {
            process_run("terminal", NULL);
        });

        _observer = navigation->observe([this](auto &) {
            _go_backward->enable_if(_navigation->can_go_backward());
            _go_foreward->enable_if(_navigation->can_go_forward());
            _go_up->enable_if(_navigation->can_go_up());
        });
    }

    ~ToolBar() override {}
};

} // namespace file_manager
