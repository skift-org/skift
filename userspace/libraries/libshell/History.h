#pragma once

#include <libshell/Line.h>

namespace Shell
{

struct History
{
private:
    Vector<Line> _history;

public:
    void commit(Line line)
    {
        _history.push(line);
    }

    const Line &peek(size_t index) const
    {
        return _history[index];
    }

    size_t length() const
    {
        return _history.count();
    }
};

} // namespace Shell
