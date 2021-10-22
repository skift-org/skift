#pragma once

#include <libwidget/Elements.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Breadcrumb.h>

namespace FilePicker
{

struct ToolBar : public Widget::PanelElement
{
private:
    Ref<Navigation> _navigation;
    Ref<Bookmarks> _bookmarks;

    Ref<Widget::Element> _go_backward;
    Ref<Widget::Element> _go_foreward;
    Ref<Widget::Element> _go_up;
    Ref<Widget::Element> _go_home;

    Ref<Widget::Element> _breadcrumb;

    Ref<Widget::Element> _refresh;
    Ref<Widget::Element> _open_terminal;

    Box<Async::Observer<Navigation>> _observer;

public:
    static constexpr int NO_HJ_OPEN_TERMINAL = 1 << 0;

    ToolBar(Ref<Navigation> navigation, Ref<Bookmarks> bookmarks, int flags = 0)
        : PanelElement(),
          _navigation(navigation),
          _bookmarks(bookmarks)
    {
        _go_backward = add(Widget::basic_button(Graphic::Icon::get("arrow-left"), [this]
            { _navigation->go_backward(); }));

        _go_foreward = add(Widget::basic_button(Graphic::Icon::get("arrow-right"), [this]
            { _navigation->go_forward(); }));

        _go_up = add(Widget::basic_button(Graphic::Icon::get("arrow-up"), [this]
            { _navigation->go_up(); }));

        _go_home = add(Widget::basic_button(Graphic::Icon::get("home"), [this]
            { _navigation->go_home(); }));

        add(Widget::separator());

        _breadcrumb = add<Breadcrumb>(_navigation, _bookmarks);
        _breadcrumb->flags(Element::FILL);

        add(Widget::separator());

        _refresh = add(Widget::basic_button(Graphic::Icon::get("refresh"), [this]
            { _navigation->refresh(); }));

        if (!(flags & NO_HJ_OPEN_TERMINAL))
        {
            add(Widget::basic_button(Graphic::Icon::get("console"), []
                { process_run("terminal", NULL, TASK_NONE); }));
        }

        _observer = _navigation->observe([this](auto &)
            {
                _go_backward->enable_if(_navigation->can_go_backward());
                _go_foreward->enable_if(_navigation->can_go_forward());
                _go_up->enable_if(_navigation->can_go_up());
            });
    }

    ~ToolBar() override {}
};

} // namespace FilePicker