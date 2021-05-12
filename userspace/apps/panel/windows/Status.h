#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct Status : public Widget::Window
{
private:
    OwnPtr<Widget::Window> _menu;
    OwnPtr<Widget::Window> _datetime;
    OwnPtr<Widget::Window> _quicksetting;

public:
    Status();

    RefPtr<Widget::Element> build() override;
};

} // namespace Panel
