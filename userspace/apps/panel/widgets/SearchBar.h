#pragma once

#include <libwidget/Components.h>
#include <libwidget/model/TextModel.h>

namespace Panel
{

struct SearchBarComponent :
    public Widget::Component
{
private:
    RefPtr<Widget::TextModel> _model;
    Callback<void(String)> _on_change;

public:
    SearchBarComponent(String text, Callback<void(String)> on_change);

    RefPtr<Widget::Element> build() override;
};

WIDGET_BUILDER(SearchBarComponent, search_bar);

} // namespace Panel
