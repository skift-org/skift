#pragma once

#include <libwidget/Component.h>

class TitleBar : public Component
{
private:
    bool _is_dragging = false;

public:
    TitleBar(Component *parent);

    ~TitleBar() override;

    void event(Event *event) override;
};
