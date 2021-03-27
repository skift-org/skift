#pragma once

#include <libsettings/Path.h>
#include <libutils/Optional.h>
#include <libutils/json/Json.h>

namespace Settings
{

/* --- Private Functions ---------------------------------------------------- */

class Watcher;

void register_watcher(Watcher &watcher);

void unregister_watcher(Watcher &watcher);

/* --- Public functions ----------------------------------------------------- */

Optional<Json::Value> read(const Path path);

bool write(const Path path, Json::Value value);

} // namespace Settings
