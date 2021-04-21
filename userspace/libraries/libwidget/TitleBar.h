#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Button.h>
#include <libwidget/Spacer.h>

namespace Widget
{

class TitleBar : public Component
{
private:
    RefPtr<Graphic::Icon> _icon;
    String _title;

    OwnPtr<Async::Invoker> _rebuild_invoker;

    bool _is_dragging = false;

public:
    String title() { return _title; }

    RefPtr<Graphic::Icon> icon() { return _icon; }

    void title(String title)
    {
        _title = title;
        should_rebuild();
    }

    void icon(RefPtr<Graphic::Icon> icon)
    {
        _icon = icon;
        should_rebuild();
    }

    TitleBar(Component *parent, RefPtr<Graphic::Icon> icon, String title);

    void event(Event *event) override;

    void should_rebuild()
    {
        _rebuild_invoker->should_be_invoke_later();
    }

    void build();
};

} // namespace Widget
