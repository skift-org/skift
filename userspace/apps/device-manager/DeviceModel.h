#pragma once

#include <libjson/Json.h>
#include <libwidget/model/TableModel.h>

struct DeviceModel : public Widget::TableModel
{
private:
    Json::Value _data = nullptr;

public:
    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;
};
