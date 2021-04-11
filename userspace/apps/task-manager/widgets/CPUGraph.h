#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class CPUGraph : public Widget::Graph
{
private:
    RefPtr<TaskModel> _model;

    Widget::Label *_label_average;
    Widget::Label *_label_greedy;
    Widget::Label *_label_uptime;

    OwnPtr<Timer> _graph_timer{};
    OwnPtr<Timer> _text_timer{};

public:
    CPUGraph(Component *parent, RefPtr<TaskModel> model);
};

} // namespace task_manager