#pragma once

#include <libwidget/Components.h>
#include <libwidget/model/TextModel.h>

namespace Panel
{

struct SearchBarComponent :
    public Widget::Component
{
private:
    Ref<Widget::TextModel> _model;
    Func<void(String)> _on_change;

public:
    SearchBarComponent(String text, Func<void(String)> on_change);

    Ref<Widget::Element> build() override;
};

WIDGET_BUILDER(SearchBarComponent, search_bar);

} // namespace Panel
