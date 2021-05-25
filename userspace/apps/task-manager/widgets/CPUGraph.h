#pragma once

#include <libasync/Timer.h>
#include <libwidget/Components.h>
#include <libwidget/Views.h>

#include "task-manager/model/TaskModel.h"

namespace TaskManager
{
struct CPUGraphComponent : public Widget::Component
{
private:
    RefPtr<TaskModel> _tasks;
    RefPtr<Widget::GraphModel> _usage;
    OwnPtr<Async::Timer> _update;

public:
    CPUGraphComponent(RefPtr<TaskModel> tasks);

    RefPtr<Element> build() override;
};

WIDGET_BUILDER(CPUGraphComponent, cpu_graph);

} // namespace TaskManager
