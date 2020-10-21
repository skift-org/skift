#pragma once

#include <libutils/String.h>

#include <libhtml/dom/Node.h>

namespace html
{

class Text : public Node
{
private:
    String _content;

public:
    Text() {}

    ~Text() {}
};

} // namespace html
