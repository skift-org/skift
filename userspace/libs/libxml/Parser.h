#pragma once
#include <libxml/Doc.h>

namespace Xml
{
ResultOr<Doc> parse(IO::Reader &reader);
}