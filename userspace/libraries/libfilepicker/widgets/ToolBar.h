#pragma once

#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Breadcrumb.h>

namespace FilePicker
{

class ToolBar : public Widget::Panel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Bookmarks> _bookmarks;

    Widget::Component *_go_backward;
    Widget::Component *_go_foreward;
    Widget::Component *_go_up;
    Widget::Component *_go_home;

    Widget::Component *_breadcrumb;

    Widget::Component *_refresh;
    Widget::Component *_open_terminal;

    OwnPtr<Async::Observer<Navigation>> _observer;

public:
    static constexpr int NO_OPEN_TERMINAL = 1 << 0;

    ToolBar(Widget::Component *parent, RefPtr<Navigation> navigation, RefPtr<Bookmarks> bookmarks, int flags = 0)
        : Panel(parent),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        layout(HFLOW(4));
        insets(Insetsi(4, 4));

        _go_backward = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("arrow-left"));

        _go_backward->on(Widget::Event::ACTION, [this](auto) {
            _navigation->go_backward();
        });

        _go_foreward = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("arrow-right"));

        _go_foreward->on(Widget::Event::ACTION, [this](auto) {
            _navigation->go_forward();
        });

        _go_up = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("arrow-up"));

        _go_up->on(Widget::Event::ACTION, [this](auto) {
            _navigation->go_up();
        });

        _go_home = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("home"));

        _go_home->on(Widget::Event::ACTION, [this](auto) {
            _navigation->go_home();
        });

        new Widget::Separator(this);

        _breadcrumb = new Breadcrumb(this, _navigation, _bookmarks);
        _breadcrumb->flags(Component::FILL);

        new Widget::Separator(this);

        _refresh = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("refresh"));

        _refresh->on(Widget::Event::ACTION, [this](auto) {
            _navigation->refresh();
        });

        if (!(flags & NO_OPEN_TERMINAL))
        {
            Component *terminal_button = new Widget::Button(this, Widget::Button::TEXT, Graphic::Icon::get("console"));

            terminal_button->on(Widget::Event::ACTION, [](auto) {
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