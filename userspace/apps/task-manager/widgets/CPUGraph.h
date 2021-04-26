#pragma once

#include <libasync/Timer.h>
#include <libwidget/Elements.h>
#include <libwidget/Graph.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class CPUGraph : public Widget::Graph
{
private:
    RefPtr<TaskModel> _model;

    RefPtr<Widget::LabelElement> _label_average;
    RefPtr<Widget::LabelElement> _label_greedy;
    RefPtr<Widget::LabelElement> _label_uptime;

    OwnPtr<Async::Timer> _graph_timer{};
    OwnPtr<Async::Timer> _text_timer{};

public:
    CPUGraph(RefPtr<TaskModel> model);
};

} // namespace task_manager