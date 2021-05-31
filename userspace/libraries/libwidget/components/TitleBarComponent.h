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
    TitleBarComponent(RefPtr<Graphic::Icon> icon, String title);

    void event(Event *event) override;

    RefPtr<Element> build() override;
};

WIDGET_BUILDER(TitleBarComponent, titlebar);

} // namespace Widget
