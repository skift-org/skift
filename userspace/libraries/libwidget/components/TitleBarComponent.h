#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Elements.h>
#include <libwidget/components/Component.h>

namespace Widget
{

struct TitleBarComponent : public Component
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

    TitleBarComponent(RefPtr<Graphic::Icon> icon, String title);

    void event(Event *event) override;

    RefPtr<Element> build() override;
};

static inline RefPtr<TitleBarComponent> titlebar(RefPtr<Graphic::Icon> icon, String title)
{
    return make<TitleBarComponent>(icon, title);
}

} // namespace Widget
