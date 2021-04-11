#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class TitleBar : public Component
{
private:
    bool _is_dragging = false;

public:
    TitleBar(Component *parent);

    ~TitleBar() override;

    void event(Event *event) override;
};

} // namespace Widget
