#pragma once

namespace Vaev::Script {

// MARK: Garbage Collector -----------------------------------------------------

struct Cell {};

// MARK: Value -----------------------------------------------------------------

struct Value {};

// MARK: Object ----------------------------------------------------------------

struct Object : public Cell {
};

struct Array : public Object {
};

struct Function : public Object {
};

struct String : public Object {
};

// MARK: Runtime ---------------------------------------------------------------

} // namespace Vaev::Script
