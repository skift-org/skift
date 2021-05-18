#pragma once

#include <libasync/Timer.h>
#include <libwidget/Window.h>

#include "task-manager/model/TaskModel.h"

namespace TaskManager
{

struct TaskManager : public Widget::Window
{
private:
    RefPtr<TaskModel> _model;
    OwnPtr<Async::Timer> _update;

public:
    TaskManager();

    RefPtr<Widget::Element> build() override;
};

} // namespace TaskManager