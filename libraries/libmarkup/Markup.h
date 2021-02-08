#pragma once

#include <libutils/Enum.h>
#include <libutils/HashMap.h>
#include <libutils/Prettifier.h>
#include <libutils/Scanner.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

namespace markup
{

using Attributes = HashMap<String, String>;

class Node
{
private:
    String _type;
    int _flags;
    Vector<Node> _childs;
    HashMap<String, String> _attributes;

public:
    static constexpr auto NONE = 0;
    static constexpr auto SELF_CLOSING = 1 << 0;

    auto type() const { return _type; }

    auto is(String type) const { return _type == type; }

    auto flags() { return _flags; }

    auto count_child() { return _childs.count(); }

    Node(String type, int flags = NONE)
        : _type(type),
          _flags(flags)
    {
    }

    Node(String type, int flags, Attributes &&attributes)
        : _type(type),
          _flags(flags),
          _attributes(attributes)
    {
    }

    bool has_attribute(String attribute)
    {
        return _attributes.has_key(attribute);
    }

    void set_attribute(String attribute, String value)
    {
        _attributes[attribute] = value;
    }

    String get_attribute(String attribute)
    {
        if (_attributes.has_key(attribute))
        {
            return _attributes[attribute];
        }
        else
        {
            return "";
        }
    }

    template <typename TCallback>
    bool with_attribute(String attribute, TCallback callback)
    {
        if (_attributes.has_key(attribute))
        {
            callback(_attributes[attribute]);
            return true;
        }
        else
        {
            return false;
        }
    }

    String get_attribute_or_default(String attribute, String fallback)
    {
        if (_attributes.has_key(attribute))
        {
            return _attributes[attribute];
        }
        else
        {
            return fallback;
        }
    }

    template <typename TCallback>
    Iteration foreach_attributes(TCallback callback)
    {
        return _attributes.foreach (callback);
    }

    void add_child(const Node &child)
    {
        _childs.push_back(child);
    }

    void add_child(Node &&child)
    {
        _childs.push_back(move(child));
    }

    template <typename TCallback>
    Iteration foreach_child(TCallback callback)
    {
        return _childs.foreach (callback);
    }
};

Node parse(Scanner &scan);

Node parse_file(String path);

void prettify(Prettifier &pretty, Node &node);

} // namespace markup
