#pragma once

#include <libutils/Callback.h>

#include <libwidget/Elements.h>
#include <libwidget/model/TextModel.h>

namespace panel
{

class SearchBar : public Widget::PanelElement
{
public:
    SearchBar(RefPtr<Widget::TextModel> model);
};

} // namespace panel
