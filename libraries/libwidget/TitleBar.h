#pragma once

#include <libwidget/Widget.h>

class TitleBar : public Widget
{
private:
    bool _is_dragging = false;

public:
    TitleBar(Widget *parent);

    ~TitleBar() override;

    void event(Event *event) override;
};
