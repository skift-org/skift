#pragma once

#include <libjson/Json.h>
#include <libwidget/model/TableModel.h>

class TaskModel : public TableModel
{
private:
    json::Value *_data = nullptr;

public:
    ~TaskModel() override;

    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;

    String ram_greedy();

    String cpu_greedy();

    void kill_task(int row);
};
