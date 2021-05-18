#pragma once

#include <libwidget/Components.h>
#include <libwidget/Views.h>

namespace TaskManager
{

struct ProcessTableComponent :
    public Widget::Component
{
private:
    RefPtr<Widget::TableModel> _model;

public:
    ProcessTableComponent(RefPtr<Widget::TableModel> model);

    RefPtr<Element> build() override;
};

WIDGET_BUILDER(ProcessTableComponent, process_table);

} // namespace TaskManager
