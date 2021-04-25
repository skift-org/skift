#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Button.h>
#include <libwidget/Component.h>
#include <libwidget/Elements.h>

namespace Widget
{

class TitleBar : public Component
{
private:
    RefPtr<Graphic::Icon> _icon;
    String _title;

    bool _is_dragging = false;

public:
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

    TitleBar(RefPtr<Graphic::Icon> icon, String title);

    void event(Event *event) override;

    RefPtr<Element> build() override;
};

static inline RefPtr<TitleBar> titlebar(RefPtr<Graphic::Icon> icon, String title)
{
    return make<TitleBar>(icon, title);
}

} // namespace Widget
