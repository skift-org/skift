#pragma once

#include "libutils/RefPtr.h"
#include "libwidget/Element.h"
#include <libwidget/model/TextModel.h>

#include <libwidget/Window.h>

namespace panel
{

class MenuWindow : public Widget::Window
{
private:
    OwnPtr<Async::Observer<Widget::TextModel>> _search_query_observer;

public:
    static constexpr int WIDTH = 320;

    MenuWindow();

    RefPtr<Widget::Element> build() override;
};

} // namespace panel
