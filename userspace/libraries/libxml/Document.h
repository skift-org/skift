#pragma once

#include <libio/Reader.h>
#include <libio/Scanner.h>
#include <libutils/String.h>
#include <libxml/Declaration.h>
#include <libxml/Node.h>

namespace Xml
{
struct Document
{
private:
    Node _root;
    Declaration _declaration;

public:
    Declaration &declaration() { return _declaration; }
    void declaration(Declaration declaration) { _declaration = declaration; }

    Node &root() { return _root; }
    void root(Node node) { _root = node; }
};
} // namespace Xml