#pragma once

#include <libasync/Timer.h>
#include <libwidget/Window.h>

#include "task-manager/model/TaskModel.h"

namespace TaskManager
{

struct TaskManager : public Widget::Window
{
private:
    Ref<TaskModel> _model;
    Box<Async::Timer> _update;

public:
    TaskManager();

    Ref<Widget::Element> build() override;
};

} // namespace TaskManager