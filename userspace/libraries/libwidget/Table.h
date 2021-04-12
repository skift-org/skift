#pragma once

#include <libutils/String.h>

#include <libwidget/ScrollBar.h>
#include <libwidget/model/TableModel.h>

namespace Widget
{

class Table : public Component
{
private:
    static constexpr int TABLE_ROW_HEIGHT = 32;

    RefPtr<TableModel> _model = nullptr;
    OwnPtr<Async::Observer<TableModel>> _model_observer;

    int _selected = -1;
    int _scroll_offset = 0;
    ScrollBar *_scrollbar;

    String _empty_message{"No data to display"};

    Math::Recti scrollbar_bound() const;
    Math::Recti header_bound() const;
    Math::Recti list_bound() const;
    Math::Recti row_bound(int row) const;
    Math::Recti column_bound(int column) const;
    Math::Recti cell_bound(int row, int column) const;
    int row_at(Math::Vec2i position) const;
    void paint_cell(Graphic::Painter &painter, int row, int column);

public:
    void model(RefPtr<TableModel> model)
    {
        _model = model;
        _model_observer = model->observe([this](auto &) {
            should_repaint();
            should_relayout();
        });
    }

    void empty_message(String message)
    {
        _empty_message = message;
    }

    int selected() { return _selected; }

    void select(int index)
    {
        if (index == _selected)
        {
            return;
        }

        if (index < -1 || index >= _model->rows())
        {
            return;
        }

        _selected = index;
        should_repaint();
    }

    void scroll_to_top()
    {
        _scroll_offset = 0;

        should_repaint();
        should_relayout();
    }

    Table(Component *parent);

    Table(Component *parent, RefPtr<TableModel> model);

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    void event(Event *event) override;

    void do_layout() override;
};

} // namespace Widget
