#pragma once

#include <libio/Reader.h>
#include <libio/Scanner.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libxml/Node.h>

namespace Xml
{
class Document
{
private:
    Node _root;

public:
    ALWAYS_INLINE Node &root() { return _root; }
};
} // namespace Xml