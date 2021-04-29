#pragma once

#include <libasync/Timer.h>
#include <libwidget/Views.h>

#include "task-manager/model/TaskModel.h"

namespace task_manager
{

class RAMGraph : public Widget::Graph
{
private:
    RefPtr<TaskModel> _model;

    RefPtr<Widget::LabelElement> _label_usage;
    RefPtr<Widget::LabelElement> _label_available;
    RefPtr<Widget::LabelElement> _label_greedy;

    OwnPtr<Async::Timer> _graph_timer{};
    OwnPtr<Async::Timer> _text_timer{};

public:
    RAMGraph(RefPtr<TaskModel> model);
};

} // namespace task_manager