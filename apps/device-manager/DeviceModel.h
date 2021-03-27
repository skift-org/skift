#pragma once

#include <libutils/json/Json.h>
#include <libwidget/model/TableModel.h>

class DeviceModel : public TableModel
{
private:
    Json::Value _data = nullptr;

public:
    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;
};
