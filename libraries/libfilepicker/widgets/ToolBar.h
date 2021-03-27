#pragma once

#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Breadcrumb.h>

namespace FilePicker
{

class ToolBar : public Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    Component *_go_backward;
    Component *_go_foreward;
    Component *_go_up;
    Component *_go_home;

    Component *_breadcrumb;

    Component *_refresh;
    Component *_open_terminal;

    OwnPtr<Observer<Navigation>> _observer;

public:
    static constexpr int NO_OPEN_TERMINAL = 1 << 0;

    ToolBar(Component *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks, int flags = 0)
        : Panel(parent),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(HFLOW(4));
        insets(Insetsi(4, 4));
        max_height(38);
        min_height(38);

        _go_backward = new Button(this, Button::TEXT, Graphic::Icon::get("arrow-left"));

        _go_backward->on(Event::ACTION, [this](auto) {
            _navigation->go_backward();
        });

        _go_foreward = new Button(this, Button::TEXT, Graphic::Icon::get("arrow-right"));

        _go_foreward->on(Event::ACTION, [this](auto) {
            _navigation->go_forward();
        });

        _go_up = new Button(this, Button::TEXT, Graphic::Icon::get("arrow-up"));

        _go_up->on(Event::ACTION, [this](auto) {
            _navigation->go_up();
        });

        _go_home = new Button(this, Button::TEXT, Graphic::Icon::get("home"));

        _go_home->on(Event::ACTION, [this](auto) {
            _navigation->go_home();
        });

        new Separator(this);

        _breadcrumb = new Breadcrumb(this, _navigation, _bookmarks);
        _breadcrumb->flags(Component::FILL);

        new Separator(this);

        _refresh = new Button(this, Button::TEXT, Graphic::Icon::get("refresh"));

        _refresh->on(Event::ACTION, [this](auto) {
            _navigation->refresh();
        });

        if (!(flags & NO_OPEN_TERMINAL))
        {
            Component *terminal_button = new Button(this, Button::TEXT, Graphic::Icon::get("console"));

            terminal_button->on(Event::ACTION, [](auto) {
                process_run("terminal", NULL, TASK_NONE);
            });
        }

        _observer = _navigation->observe([this](auto &) {
            _go_backward->enable_if(_navigation->can_go_backward());
            _go_foreward->enable_if(_navigation->can_go_forward());
            _go_up->enable_if(_navigation->can_go_up());
        });
    }

    ~ToolBar() override {}
};

} // namespace FilePicker