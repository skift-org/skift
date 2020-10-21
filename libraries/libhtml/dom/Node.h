#pragma once

#include <libutils/Vector.h>

namespace html
{

class Node : public RefCounted<Node>
{
private:
    Vector<RefPtr<Node>> _childs{};

public:
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
