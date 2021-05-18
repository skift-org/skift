#pragma once

#include <libasync/Observable.h>
#include <libgraphic/Color.h>
#include <libutils/RefCounted.h>

namespace Widget
{

struct GraphModel :
    public RefCounted<GraphModel>,
    public Async::Observable<GraphModel>
{
private:
    Vector<float> _data{};
    size_t _current = 0;

public:
    GraphModel(size_t size = 100)
    {
        _data.resize(size);
    }

    float current() const
    {
        return _current / (float)_data.count();
    }

    const Vector<float> &data() const
    {
        return _data;
    }

    float sample(float where)
    {
        assert(where >= 0 && where <= 1);
        return _data[(size_t)(_data.count() * where)];
    }

    void record(float data)
    {
        _data[_current] = data;
        _current = (_current + 1) % _data.count();

        did_update();
    }

    float average()
    {
        if (_data.count() == 0)
        {
            return 0;
        }

        float total = 0;

        for (size_t i = 0; i < MIN(_current, _data.count()); i++)
        {
            total += _data[i];
        }

        return total / MIN(_current, _data.count());
    }
};

} // namespace Widget
