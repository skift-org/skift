#pragma once

#include <libwidget/Components.h>
#include <libwidget/Views.h>

namespace TaskManager
{

struct ProcessTableComponent :
    public Widget::Component
{
private:
    Ref<Widget::TableModel> _model;

public:
    ProcessTableComponent(Ref<Widget::TableModel> model);

    Ref<Element> build() override;
};

WIDGET_BUILDER(ProcessTableComponent, process_table);

} // namespace TaskManager
