#pragma once
#include <libutils/HashMap.h>
#include <libutils/Vector.h>

namespace Xml
{
class Node
{
private:
    Vector<Node> _children;
    String _content;
    String _name;
    HashMap<String, String> _attributes;

public:
    String &content() { return _content; }
    String &name() { return _name; }
    Vector<Node> &children() { return _children; }
    HashMap<String, String> &attributes() { return _attributes; }
};
} // namespace Xml