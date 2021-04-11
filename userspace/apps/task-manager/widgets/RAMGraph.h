#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Graph.h>
#include <libwidget/Label.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class RAMGraph : public Widget::Graph
{
private:
    RefPtr<TaskModel> _model;

    Widget::Label *_label_usage;
    Widget::Label *_label_available;
    Widget::Label *_label_greedy;

    OwnPtr<Timer> _graph_timer{};
    OwnPtr<Timer> _text_timer{};

public:
    RAMGraph(Component *parent, RefPtr<TaskModel> model);
};

} // namespace task_manager