#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class CPUGraph : public Graph
{
private:
    RefPtr<TaskModel> _model;

    Label *_label_average;
    Label *_label_greedy;
    Label *_label_uptime;

    OwnPtr<Timer> _graph_timer{};
    OwnPtr<Timer> _text_timer{};

public:
    CPUGraph(Widget *parent, RefPtr<TaskModel> model);
};

} // namespace task_manager