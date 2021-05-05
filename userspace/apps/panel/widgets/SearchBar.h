#pragma once

#include "libutils/Callback.h"
#include "libutils/RefPtr.h"
#include "libwidget/Element.h"
#include <libwidget/Components.h>
#include <libwidget/model/TextModel.h>

namespace panel
{

class SearchBar : public Widget::Component
{
private:
    RefPtr<Widget::TextModel> _model;
    Callback<void(String)> _on_change;

public:
    SearchBar(String text, Callback<void(String)> on_change);

    RefPtr<Widget::Element> build() override;
};

WIDGET_BUILDER(SearchBar, search_bar);

} // namespace panel
