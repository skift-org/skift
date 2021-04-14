#pragma once
#include <libutils/Vector.h>

namespace Xml
{
class Node
{
private:
    Vector<Node> _children;
    String _content;
    String _name;

public:
    String &content() { return _content; }
    String &name() { return _name; }
    Vector<Node> &children() { return _children; }
};
} // namespace Xml