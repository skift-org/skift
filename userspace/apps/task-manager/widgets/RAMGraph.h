#pragma once

#include <libasync/Timer.h>
#include <libwidget/Components.h>
#include <libwidget/Views.h>

#include "task-manager/model/TaskModel.h"

namespace TaskManager
{

struct RAMGraphComponent : public Widget::Component
{
private:
    Ref<TaskModel> _tasks;
    Ref<Widget::GraphModel> _usage;
    Box<Async::Timer> _update;

public:
    RAMGraphComponent(Ref<TaskModel> tasks);

    Ref<Element> build() override;
};

WIDGET_BUILDER(RAMGraphComponent, ram_graph);

} // namespace TaskManager
