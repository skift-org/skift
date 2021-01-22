#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class RAMGraph : public Graph
{
private:
    RefPtr<TaskModel> _model;

    Label *_label_usage;
    Label *_label_available;
    Label *_label_greedy;

    OwnPtr<Timer> _graph_timer{};
    OwnPtr<Timer> _text_timer{};

public:
    RAMGraph(Widget *parent, RefPtr<TaskModel> model);
};

} // namespace task_manager