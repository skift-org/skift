#pragma once

#include <libio/Reader.h>
#include <libio/Scanner.h>
#include <libutils/String.h>
#include <libxml/Decl.h>
#include <libxml/Node.h>

namespace Xml
{

struct Doc
{
private:
    Node _root;
    Decl _decl;

public:
    Decl &decl() { return _decl; }
    void decl(Decl decl) { _decl = decl; }

    Node &root() { return _root; }
    void root(Node node) { _root = node; }
};

} // namespace Xml