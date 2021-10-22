#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct QuickSettings : public Widget::Window
{
    QuickSettings();
    Ref<Widget::Element> build() override;
};

} // namespace Panel
