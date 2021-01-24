#pragma once

#include <libwidget/model/TextModel.h>

#include <libwidget/Window.h>

namespace panel
{

class MenuWindow : public Window
{
private:
    OwnPtr<Observer<TextModel>> _search_query_observer;

public:
    static constexpr int WIDTH = 320;

    MenuWindow();
};

} // namespace panel
