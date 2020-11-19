#include <libmarkup/Markup.h>
#include <libutils/StringBuilder.h>

namespace markup
{

void prettify(Prettifier &pretty, Node &node)
{
    pretty.append('<');

    pretty.color_depth();
    pretty.append(node.type());
    pretty.color_clear();

    node.foreach_attributes([&](auto &key, auto &value) {
        pretty.append(' ');
        pretty.append(key);

        if (value != "")
        {
            pretty.append('=');
            pretty.append('"');
            pretty.append(value);
            pretty.append('"');
        }

        return Iteration::CONTINUE;
    });

    if (node.count_child() == 0)
    {
        pretty.append("/>");
        return;
    }
    else
    {
        pretty.append('>');
    }

    node.foreach_child([&](auto &child) {
        pretty.push_ident();
        pretty.ident();

        prettify(pretty, child);

        pretty.pop_ident();

        return Iteration::CONTINUE;
    });

    pretty.ident();

    pretty.append("</");

    pretty.color_depth();
    pretty.append(node.type());
    pretty.color_clear();

    pretty.append('>');
}

} // namespace markup
