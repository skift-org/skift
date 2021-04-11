#pragma once

#include <libwidget/model/TextModel.h>

#include <libwidget/Window.h>

namespace panel
{

class MenuWindow : public Widget::Window
{
private:
    OwnPtr<Observer<Widget::TextModel>> _search_query_observer;

public:
    static constexpr int WIDTH = 320;

    MenuWindow();
};

} // namespace panel
