#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Elements.h>
#include <libwidget/components/Component.h>

namespace Widget
{

struct TitleBarComponent : public Component
{
private:
    Ref<Graphic::Icon> _icon;
    String _title;
    bool _is_dragging = false;

public:
    TitleBarComponent(Ref<Graphic::Icon> icon, String title);

    void event(Event *event) override;

    Ref<Element> build() override;
};

WIDGET_BUILDER(TitleBarComponent, titlebar);

} // namespace Widget
