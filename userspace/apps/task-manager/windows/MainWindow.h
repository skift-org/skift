#pragma once

#include <libwidget/Table.h>
#include <libwidget/Window.h>

#include "task-manager/widgets/CPUGraph.h"
#include "task-manager/widgets/RAMGraph.h"

namespace task_manager
{

class MainWinow : public Widget::Window
{
private:
    RefPtr<RAMGraph> _ram_graph;
    RefPtr<CPUGraph> _cpu_graph;
    RefPtr<Widget::Table> _table;
    RefPtr<TaskModel> _table_model;
    OwnPtr<Async::Timer> _table_timer;

public:
    MainWinow();
};

} // namespace task_manager
