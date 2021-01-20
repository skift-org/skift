#pragma once

#include <libsettings/Path.h>
#include <libsystem/json/Json.h>
#include <libutils/Optional.h>

namespace settings
{

/* --- Private Functions ---------------------------------------------------- */

class Watcher;

void register_watcher(Watcher &watcher);

void unregister_watcher(Watcher &watcher);

/* --- Public functions ----------------------------------------------------- */

Optional<json::Value> read(const Path path);

bool write(const Path path, json::Value value);

} // namespace settings
