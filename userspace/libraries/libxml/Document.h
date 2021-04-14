#pragma once

#include <libio/Reader.h>
#include <libio/Scanner.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libxml/Declaration.h>
#include <libxml/Node.h>

namespace Xml
{
class Document
{
private:
    Node _root;
    Declaration _decl;

public:
    ALWAYS_INLINE Node &root() { return _root; }
    ALWAYS_INLINE Declaration &declaration() { return _decl; }
};
} // namespace Xml