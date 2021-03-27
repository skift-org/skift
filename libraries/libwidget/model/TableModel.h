#pragma once

#include <libgraphic/Color.h>
#include <libutils/Observable.h>
#include <libwidget/utils/Variant.h>

namespace Widget
{

class TableModel :
    public RefCounted<TableModel>,
    public Observable<TableModel>
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
        __unused(column);

        return "<no header>";
    }

    virtual Variant data(int row, int column)
    {
        __unused(row);
        __unused(column);

        return "<no data>";
    }

    virtual void update()
    {
        ASSERT_NOT_REACHED();
    }
};

} // namespace Widget
