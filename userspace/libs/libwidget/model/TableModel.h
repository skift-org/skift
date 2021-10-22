#pragma once

#include <libasync/Observable.h>
#include <libgraphic/Color.h>
#include <libwidget/utils/Var.h>

namespace Widget
{

struct TableModel :
    public Shared<TableModel>,
    public Async::Observable<TableModel>
{
public:
    TableModel() {}

    virtual ~TableModel() {}

    virtual int rows()
    {
        return 0;
    }

    virtual int columns()
    {
        return 1;
    }

    virtual String header(int column)
    {
        UNUSED(column);

        return "<no header>";
    }

    virtual Var data(int row, int column)
    {
        UNUSED(row);
        UNUSED(column);

        return "<no data>";
    }

    virtual void update()
    {
        ASSERT_NOT_REACHED();
    }
};

} // namespace Widget
