#pragma once

#include <libutils/Vector.h>

namespace html
{

class Node : public RefCounted<Node>
{
private:
    Vector<RefPtr<Node>> _childs{};
    bool _self_closing = false;

public:
    bool self_closing() { return _self_closing; }
    void self_closing(bool v) { _self_closing = v; }

    Node()
    {
    }

    ~Node()
    {
    }

    bool operator==(const Node &other)
    {
        return this == &other;
    }

    void append(RefPtr<Node> child)
    {
        if (!_childs.contains(child))
        {
            _childs.push_back(child);
        }
    }
};

} // namespace html
