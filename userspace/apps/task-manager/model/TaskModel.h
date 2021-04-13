#pragma once

#include <libjson/Json.h>
#include <libwidget/model/TableModel.h>

namespace task_manager
{

class TaskModel : public Widget::TableModel
{
private:
    Json::Value _data;

public:
    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;

    String ram_greedy();

    String cpu_greedy();

    Result kill_task(int row);
};

}; // namespace task_manager