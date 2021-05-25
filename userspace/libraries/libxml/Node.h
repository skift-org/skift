#pragma once

#include <libutils/HashMap.h>
#include <libutils/Vector.h>

namespace Xml
{

struct Node
{
private:
    Vector<Node> _children;
    String _content;
    String _name;
    HashMap<String, String> _attributes;

public:
    const String &name() const { return _name; }
    void name(String name) { _name = name; }

    const String &content() const { return _content; }
    void content(String content) { _content = content; }

    Vector<Node> &children() { return _children; }
    HashMap<String, String> &attributes() { return _attributes; }

    const String inner_text() const
    {
        IO::MemoryWriter memory;
        IO::write(memory, content());
        for (auto &child : _children)
        {
            IO::write(memory, child.inner_text());
        }
        return memory.string();
    }
};

} // namespace Xml