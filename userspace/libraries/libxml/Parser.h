#pragma once
#include <libxml/Document.h>

namespace Xml
{
ResultOr<Document> parse(IO::Reader &reader);
}